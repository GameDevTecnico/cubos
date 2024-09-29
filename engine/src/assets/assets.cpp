#include <regex>
#include <utility>

#include <nlohmann/json.hpp>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/assets.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Assets)
{
    return Type::create("cubos::engine::Assets").with(ConstructibleTrait::typed<Assets>().build());
}

Assets::Assets()
{
    // Initialize the UUID generator.
    std::random_device rd;
    auto seedData = std::array<int, std::mt19937::state_size>{};
    std::generate(seedData.begin(), seedData.end(), std::ref(rd));
    std::seed_seq seq(seedData.begin(), seedData.end());
    mRandom = std::mt19937(seq);

    // Spawn the loaded thread.
    mLoaderShouldExit = false;
    mLoaderThread = std::thread([this]() { this->loader(); });
}

Assets::~Assets()
{
    // Signal the loader thread to exit.
    {
        std::unique_lock loaderLock(mLoaderMutex);
        mLoaderShouldExit = true;
        mLoaderCond.notify_one();
    }

    // Wait for the loader thread to exit.
    mLoaderThread.join();

    // Destroy all assets.
    for (auto& entry : mEntries)
    {
        if (entry.second->status == Assets::Status::Loaded)
        {
            entry.second->destructor(entry.second->data);
        }
    }
}

void Assets::registerBridge(const std::string& extension, std::shared_ptr<AssetBridge> bridge)
{
    std::unique_lock lock(mMutex);

    if (mBridges.contains(extension))
    {
        CUBOS_ERROR("Can't register asset bridge: extension {} already registered", extension);
        return;
    }

    mBridges.emplace(extension, std::move(bridge));

    CUBOS_TRACE("Registered asset bridge for extension {}", extension);
}

void Assets::cleanup()
{
    std::shared_lock lock(mMutex);

    for (const auto& entry : mEntries)
    {
        std::unique_lock assetLock(entry.second->mutex);

        if (entry.second->status == Status::Loaded && entry.second->refCount == 0)
        {
            entry.second->status = Status::Unloaded;
            entry.second->destructor(entry.second->data);
            entry.second->data = nullptr;
            CUBOS_DEBUG("Unloaded asset {}", AnyAsset(entry.first));
        }
    }
}

void Assets::loadMeta(std::string_view path)
{
    auto file = core::data::FileSystem::find(path);
    if (file == nullptr)
    {
        CUBOS_ERROR("Couldn't load asset metadata: file {} not found", path);
        return;
    }

    if (file->directory())
    {
        auto child = file->child();
        while (child != nullptr)
        {
            this->loadMeta(child->path());
            child = child->sibling();
        }
    }
    else if (file->name().ends_with(".meta"))
    {
        CUBOS_DEBUG("Loading asset metadata from {}", path);

        // Read the file contents into a string.
        std::string contents;
        {
            auto stream = file->open(core::data::File::OpenMode::Read);
            stream->readUntil(contents, nullptr);
        }

        // Load the asset metadata from the JSON string.
        auto meta = AssetMeta();
        nlohmann::json json;

        try
        {
            json = nlohmann::json::parse(contents);
        }
        catch (const nlohmann::json::parse_error&)
        {
            CUBOS_ERROR("Couldn't load asset metadata: JSON parse failed for file {}", path);
            return;
        }

        for (const auto& [key, value] : json.items())
        {
            meta.set(key, value.get<std::string>());
        }

        // Check if the metadata has a path field, which is always ignored.
        if (meta.get("path").has_value())
        {
            CUBOS_WARN("Asset metadata at {} has a path field, which is always ignored, since it is derived from the "
                       "file path",
                       path);
        }

        // Get the asset's path from the metadata path - excluding the .meta.
        auto pathWithoutMeta = path.substr(0, path.size() - 5);
        uuids::uuid id;

        // Get the UUID from the metadata, if it exists.
        if (meta.get("id").has_value())
        {
            id = uuids::uuid::from_string(meta.get("id").value()).value_or(uuids::uuid());
        }

        // If the UUID is invalid, generate a new random one.
        if (id.is_nil())
        {
            CUBOS_WARN("Asset metadata at {} has an unspecified/invalid UUID, generating a random one", path);
            id = uuids::uuid_random_generator(mRandom.value())();
        }

        // Update the metadata with the patth and UUID.
        meta.set("path", pathWithoutMeta);
        meta.set("id", uuids::to_string(id));

        // Create a handle for the asset and store its metadata.
        auto handle = AnyAsset(id);
        {
            auto guard = this->writeMeta(handle);
            *guard = meta;
            // Force the asset to be reloaded. No need to lock it again, the guard is already write
            // locking the asset.
            this->invalidate(handle, false);
        }

        CUBOS_DEBUG("Loaded asset {} metadata from {}", handle, path);
    }
}

AnyAsset Assets::load(AnyAsset handle) const
{
    auto assetEntry = this->entry(handle);
    CUBOS_INFO("Asset::load Loading asset with ID: {} and Path: {}", assetEntry->meta.get("id").value_or(""),
               assetEntry->meta.get("path").value_or(""));
    if (assetEntry == nullptr)
    {
        CUBOS_ERROR("Could not load asset");
        return {};
    }

    if (assetEntry->status != Assets::Status::Loaded)
    {
        // Find a bridge for the asset.
        auto bridge = this->bridge(handle);
        if (bridge == nullptr)
        {
            CUBOS_ERROR("Could not load asset");
            return {};
        }

        // Check if the bridge supports asynchronous loading. If it is, queue the asset for loading.
        if (bridge->asynchronous())
        {
            // We need to lock this to prevent the asset from being queued twice by a concurrent thread.
            // We also check if this is being called from the loader thread, in which case we don't need
            // to queue the asset.
            std::unique_lock lock(mLoaderMutex);
            if (assetEntry->status == Assets::Status::Unloaded && std::this_thread::get_id() != mLoaderThread.get_id())
            {
                CUBOS_TRACE("Queuing asset {} for loading", handle);
                assetEntry->status = Assets::Status::Loading;
                mLoaderQueue.push_back(Task{handle, bridge});
                mLoaderCond.notify_one();
            }
            lock.unlock();
        }
    }

    // Return a strong handle to the asset.
    assetEntry->refCount += 1;
    handle.mRefCount = &assetEntry->refCount;
    handle.mId = uuids::uuid::from_string(assetEntry->meta.get("id").value_or("")).value_or(uuids::uuid());
    CUBOS_INFO("New Handle ID: {} and Path: {}", uuids::to_string(handle.mId),
               assetEntry->meta.get("path").value_or(""));
    return handle;
}

bool Assets::saveMeta(const AnyAsset& handle) const
{
    // Get the asset metadata.
    auto meta = this->readMeta(handle);
    if (auto path = meta->get("path"))
    {
        CUBOS_DEBUG("Saving metadata for asset {}", handle);

        auto file = core::data::FileSystem::create(*path + ".meta");
        if (file == nullptr)
        {
            CUBOS_ERROR("Could not save asset: could not create file '{}.meta'", *path);
            return false;
        }

        auto stream = file->open(core::data::File::OpenMode::Write);
        if (stream == nullptr)
        {
            CUBOS_ERROR("Could not save asset: could not open file '{}.meta'", *path);
            return false;
        }

        nlohmann::json json;
        for (const auto& [key, value] : meta->params())
        {
            if (key != "path")
            {
                json[key] = value;
            }
        }
        stream->print(json.dump(4));
        return true;
    }

    CUBOS_ERROR("Could not save asset: assets without paths cannot be saved");
    return false;
}

bool Assets::save(const AnyAsset& handle) const
{
    // Save the asset's metadata.
    if (!this->saveMeta(handle))
    {
        return false;
    }

    // Find a bridge to be used with this asset.
    auto bridge = this->bridge(handle);
    if (bridge == nullptr)
    {
        CUBOS_ERROR("Could not save asset");
        return false;
    }

    // Call the bridge's save method.
    CUBOS_DEBUG("Saving data for asset {}", handle);
    return bridge->save(*this, handle);
}

Assets::Status Assets::status(const AnyAsset& handle) const
{
    std::shared_lock lock(mMutex);

    // Do not use .entry() here because we don't want to log errors if the asset is unknown.
    auto it = mEntries.find(uuids::uuid::from_string(handle.idOrPath).value_or(uuids::uuid()));
    if (it == mEntries.end())
    {
        return Status::Unknown;
    }

    return it->second->status;
}

bool Assets::update(AnyAsset& handle) const
{
    auto assetEntry = this->entry(handle);
    if (assetEntry == nullptr)
    {
        CUBOS_ERROR("Could not update asset handle");
        return false;
    }

    if (handle.mId != uuids::uuid::from_string(handle.idOrPath).value_or(uuids::uuid()))
    {
        handle = AnyAsset{handle.idOrPath};
        handle.mVersion = assetEntry->version;
        return true;
    }

    if (assetEntry->version > handle.getVersion())
    {
        handle.mVersion = assetEntry->version;
        return true;
    }

    return false;
}

void Assets::invalidate(const AnyAsset& handle)
{
    this->invalidate(handle, true);
}

void Assets::invalidate(const AnyAsset& handle, bool shouldLock)
{
    auto assetEntry = this->entry(handle);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not invalidate asset");

    // Lock the asset to make sure it isn't being used by another thread (if requested).
    std::unique_lock<std::shared_mutex> lock(assetEntry->mutex, std::defer_lock);
    if (shouldLock)
    {
        lock.lock();
    }

    // If it has associated data, free it.
    if (assetEntry->data != nullptr)
    {
        CUBOS_ASSERT(assetEntry->destructor != nullptr, "No destructor registered for asset type {}",
                     assetEntry->type->name());
        assetEntry->destructor(assetEntry->data);
        assetEntry->data = nullptr;
        assetEntry->status = Status::Unloaded;
        assetEntry->version++;

        CUBOS_DEBUG("Invalidated asset {}", handle);
    }
}

AssetMetaRead Assets::readMeta(const AnyAsset& handle) const
{
    CUBOS_WARN("Reading metadata for asset {}", handle);
    auto assetEntry = this->entry(handle);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not access metadata");

    auto lock = std::shared_lock(assetEntry->mutex);
    return {assetEntry->meta, std::move(lock)};
}

AssetMetaWrite Assets::writeMeta(const AnyAsset& handle)
{
    auto assetEntry = this->entry(handle, true);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not access metadata");

    auto lock = std::unique_lock(assetEntry->mutex);
    return {assetEntry->meta, std::move(lock)};
}

Assets::Entry::Entry()
    : refCount(0)
{
}

AnyAsset Assets::create(const Type& type, void* data, void (*destructor)(void*))
{
    // Generate a new UUID and store the asset.
    auto id = uuids::uuid_random_generator(mRandom.value())();
    return this->store(AnyAsset(id), type, data, destructor);
}

AnyAsset Assets::store(AnyAsset handle, const Type& type, void* data, void (*destructor)(void*))
{
    // Get or create a new entry for the asset.
    auto assetEntry = this->entry(handle, true);
    if (assetEntry == nullptr)
    {
        CUBOS_ERROR("Could not store asset");
        return {};
    }

    // Lock the asset.
    std::unique_lock lock(assetEntry->mutex);

    // If it has associated data, free it.
    if (assetEntry->data != nullptr)
    {
        CUBOS_ASSERT(assetEntry->destructor != nullptr, "No destructor registered for asset type {}",
                     assetEntry->type->name());
        assetEntry->destructor(assetEntry->data);
    }

    // Mark it as loaded and set its data.
    assetEntry->status = Status::Loaded;
    assetEntry->version++;
    assetEntry->data = data;
    assetEntry->type = &type;
    assetEntry->destructor = destructor;
    assetEntry->cond.notify_all();

    CUBOS_DEBUG("Stored data of type {} for asset {}", type.name(), handle);

    // Return a strong handle to the asset.
    assetEntry->refCount.fetch_add(1);
    handle.mRefCount = &assetEntry->refCount;
    handle.mVersion = assetEntry->version;
    return handle;
}

template <typename Lock>
void* Assets::access(const AnyAsset& handle, const Type& type, Lock& lock, bool incVersion) const
{
    CUBOS_ASSERT(!handle.isNull(), "Could not access asset");

    // Get the entry for the asset.
    auto assetEntry = this->entry(handle);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not access asset");

    // If the asset hasn't been loaded yet, and its bridge isn't asynchronous, or we're in the loader thread, then load
    // it now.
    if (assetEntry->status != Status::Loaded)
    {
        auto bridge = this->bridge(handle);
        CUBOS_ASSERT(bridge != nullptr, "Could not access asset");

        if (!bridge->asynchronous() || std::this_thread::get_id() == mLoaderThread.get_id())
        {
            CUBOS_DEBUG("Loading asset {} as a dependency", handle);

            // Load the asset - the const_cast is okay since the const qualifiers are only used to make
            // the interface more readable. We need to unlock temporarily to avoid a deadlock, since the
            // bridge will call back into the asset manager.
            lock.unlock();
            if (!bridge->load(const_cast<Assets&>(*this), handle))
            {
                CUBOS_CRITICAL("Could not load asset {}", handle);
                abort();
            }
            lock.lock();
        }
    }

    // Wait until the asset finishes loading.
    while (assetEntry->status == Status::Loading)
    {
        assetEntry->cond.wait(lock);
    }

    CUBOS_ASSERT(assetEntry->status == Status::Loaded && assetEntry->data != nullptr, "Could not access asset");
    CUBOS_ASSERT(assetEntry->type == &type, "Type mismatch, expected {}, got {}", type.name(),
                 assetEntry->type->name());

    if (incVersion)
    {
        assetEntry->version++;
        CUBOS_DEBUG("Incremented version of asset {}", handle);
    }

    return assetEntry->data;
}

// Define the explicit instantiations of the access() function, one for each lock type.
// Necessary because the function is template - would otherwise cause linker errors.

template void* Assets::access<std::shared_lock<std::shared_mutex>>(const AnyAsset&, const Type&,
                                                                   std::shared_lock<std::shared_mutex>&, bool) const;
template void* Assets::access<std::unique_lock<std::shared_mutex>>(const AnyAsset&, const Type&,
                                                                   std::unique_lock<std::shared_mutex>&, bool) const;

std::shared_lock<std::shared_mutex> Assets::lockRead(const AnyAsset& handle) const
{
    if (auto entry = this->entry(handle))
    {
        return std::shared_lock(entry->mutex);
    }

    CUBOS_CRITICAL("Couldn't lock asset for reading");
    abort();
}

std::unique_lock<std::shared_mutex> Assets::lockWrite(const AnyAsset& handle) const
{
    if (auto entry = this->entry(handle))
    {
        return std::unique_lock(entry->mutex);
    }

    CUBOS_CRITICAL("Couldn't lock asset for writing");
    abort();
}

std::string Assets::checkIdType(const std::string& id) const
{
    if (uuids::uuid::from_string(id).has_value())
    {
        return "UUID";
    }
    else if (id.find('/') != std::string::npos || id.find('\\') != std::string::npos)
    {
        return "Path";
    }
    else
    {
        return "Unknown";
    }
}

std::shared_ptr<Assets::Entry> Assets::entry(const AnyAsset& handle) const
{
    // If the handle is null, we can't access the asset.
    if (handle.isNull())
    {
        CUBOS_ERROR("Null asset handle");
        return nullptr;
    }

    // Lock the entries map for reading.
    auto sharedLock = std::shared_lock(mMutex);
    // Search for the entry in the map.
    std::string id = handle.getId();
    uuids::uuid uuid = uuids::uuid{};
    CUBOS_ERROR("Assets::entry 1 Current ID: {}", id);
    CUBOS_INFO("Assets::entry 1 Current ID type: {}", checkIdType(id));
    if (Assets::checkIdType(id) == "Path")
    {
        for (auto asset : mEntries)
        {
            auto meta = asset.second->meta;
            auto path = meta.get("path");
            auto idd = meta.get("id");
            CUBOS_ERROR("Assets::entry Path Current path: {}", path.value());
            CUBOS_WARN("Assets::entry Path Current ID: {}", idd.value());
            if (path.has_value() && path.value() == id)
            {
                CUBOS_INFO("Assets::entry Path Found asset with path: {}", id);
                uuid = uuids::uuid::from_string(meta.get("id").value()).value();
                CUBOS_INFO("Assets::entry Path Found asset with id: {}", uuids::to_string(uuid));
                break;
            }
        }
    }
    else if (Assets::checkIdType(id) == "UUID")
    {
        uuid = uuids::uuid::from_string(id).value();
    }
    else
    {
        CUBOS_ERROR("Type mismatch {}", handle);
        return nullptr;
    }
    auto it = mEntries.find(uuid);
    // CUBOS_INFO("Assets::entry 1 Found asset with id: {}, and Path: {}", uuids::to_string(it->first),
    //            it->second->meta.get("path").value());
    CUBOS_INFO("Assets::entry 1 UUID to find id: {}", uuids::to_string(uuid));
    if (it == mEntries.end())
    {
        CUBOS_ERROR("No such asset found with the uuid:  {}", handle);
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<Assets::Entry> Assets::entry(const AnyAsset& handle, bool create)
{
    // If the handle is null, we can't access the asset.
    if (handle.isNull())
    {
        CUBOS_ERROR("Null asset handle");
        return nullptr;
    }

    // If we're creating the asset, we need to lock the entries map for writing.
    // Otherwise, we can lock it for reading.
    auto uniqueLock = std::unique_lock(mMutex, std::defer_lock);
    auto sharedLock = std::shared_lock(mMutex, std::defer_lock);
    if (create)
    {
        uniqueLock.lock();
    }
    else
    {
        sharedLock.lock();
    }

    // Search for an existing entry for the asset.

    std::string id = handle.getId();
    uuids::uuid uuid = uuids::uuid{};
    if (Assets::checkIdType(id) == "Path")
    {
        for (auto asset : mEntries)
        {
            auto meta = asset.second->meta;
            auto path = meta.get("path");
            if (path.has_value() && path.value() == id)
            {
                uuid = uuids::uuid::from_string(meta.get("id").value()).value();
                break;
            }
        }
    }
    else if (Assets::checkIdType(id) == "UUID")
    {
        uuid = uuids::uuid::from_string(id).value();
    }
    else
    {
        CUBOS_ERROR("Id type missmatch", id);
        return nullptr;
    }
    auto it = mEntries.find(uuid);
    if (it == mEntries.end())
    {
        // If we're creating the asset, create a new entry for it.
        // Otherwise, return nullptr.
        if (create)
        {
            auto entry = std::make_shared<Entry>();
            entry->meta.set("id", uuids::to_string(uuid));
            it = mEntries.emplace(uuid, std::move(entry)).first;
            CUBOS_TRACE("Created new asset entry for {}", handle);
        }
        else
        {
            CUBOS_ERROR("No such asset {}", handle);
            return nullptr;
        }
    }
    return it->second;
}

std::shared_ptr<AssetBridge> Assets::bridge(const AnyAsset& handle, bool logError) const
{
    auto meta = this->readMeta(handle);
    if (auto path = meta->get("path"))
    {
        std::shared_lock lock(mMutex);

        // Get the bridge which has the best match for this asset.
        std::shared_ptr<AssetBridge> best = nullptr;
        std::size_t bestLen = 0;
        for (const auto& bridge : mBridges)
        {
            if (path->ends_with(bridge.first) && bridge.first.length() > bestLen)
            {
                best = bridge.second;
                bestLen = bridge.first.length();
            }
        }

        if (best == nullptr && logError)
        {
            CUBOS_ERROR("Cannot find a matching bridge for asset path {}", *path);
        }

        return best;
    }
    if (logError)
    {
        CUBOS_ERROR("Cannot find a bridge for asset {}: asset does not have a path", handle);
    }
    return nullptr;
}

void Assets::loader()
{
    for (;;)
    {
        // Wait for a new asset to load.
        std::unique_lock<std::mutex> loaderLock(mLoaderMutex);
        mLoaderCond.wait(loaderLock, [this]() { return !mLoaderQueue.empty() || mLoaderShouldExit; });

        // If the loader thread should exit, exit.
        if (mLoaderShouldExit)
        {
            return;
        }

        // Get the next asset to load.
        auto task = mLoaderQueue.front();
        mLoaderQueue.pop_front();
        loaderLock.unlock(); // Unlock the mutex before loading the asset.

        if (!task.bridge->load(*this, task.handle))
        {
            CUBOS_ERROR("Failed to load asset {}", task.handle);

            auto assetEntry = this->entry(task.handle);
            CUBOS_ASSERT(assetEntry != nullptr, "This should never happen");
            assetEntry->status = Assets::Status::Unloaded;
            assetEntry->cond.notify_all();
        }
        else
        {
            auto assetEntry = this->entry(task.handle);
            CUBOS_ASSERT(assetEntry != nullptr);
            CUBOS_ASSERT(assetEntry->type == &task.bridge->assetType());
        }
    }
}

std::vector<AnyAsset> Assets::listAll() const
{
    std::vector<AnyAsset> out;
    for (const auto& [entry, _] : mEntries)
    {
        out.emplace_back(entry);
    }
    return out;
}

const Type& Assets::type(const AnyAsset& handle) const
{
    auto assetEntry = this->entry(handle);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not find asset's type");
    if (assetEntry->status == Status::Loaded)
    {
        return *assetEntry->type;
    }

    auto bridge = this->bridge(handle);
    CUBOS_ASSERT(bridge != nullptr, "Could not find asset's type");
    return bridge->assetType();
}

bool Assets::hasKnownType(const AnyAsset& handle) const
{
    auto assetEntry = this->entry(handle);
    if (assetEntry == nullptr)
    {
        return false;
    }

    if (assetEntry->status == Status::Loaded)
    {
        return true;
    }

    return this->bridge(handle, false) != nullptr;
}