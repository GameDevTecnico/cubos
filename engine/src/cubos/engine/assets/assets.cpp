#include <utility>

#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/json_deserializer.hpp>
#include <cubos/core/data/json_serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/assets.hpp>

using namespace cubos::engine;

Assets::Assets()
{
    // Initialize the UUID generator.
    std::random_device rd;
    auto seedData = std::array<int, std::mt19937::state_size>{};
    std::generate(seedData.begin(), seedData.end(), std::ref(rd));
    std::seed_seq seq(seedData.begin(), seedData.end());
    this->random = std::mt19937(seq);

    // Spawn the loaded thread.
    this->loaderShouldExit = false;
    this->loaderThread = std::thread([this]() { this->loader(); });
}

Assets::~Assets()
{
    // Signal the loader thread to exit.
    {
        std::unique_lock loaderLock(this->loaderMutex);
        this->loaderShouldExit = true;
        this->loaderCond.notify_one();
    }

    // Wait for the loader thread to exit.
    this->loaderThread.join();

    // Destroy all assets.
    for (auto& entry : this->entries)
    {
        if (entry.second->status == Assets::Status::Loaded)
        {
            entry.second->destructor(entry.second->data);
        }
    }
}

void Assets::registerBridge(const std::string& extension, std::shared_ptr<AssetBridge> bridge)
{
    std::unique_lock lock(this->mutex);

    if (this->bridges.contains(extension))
    {
        CUBOS_ERROR("Can't register asset bridge: extension '{}' already registered", extension);
        return;
    }

    this->bridges.emplace(extension, std::move(bridge));

    CUBOS_TRACE("Registered asset bridge for extension '{}'", extension);
}

void Assets::cleanup()
{
    std::shared_lock lock(this->mutex);

    for (const auto& entry : this->entries)
    {
        std::unique_lock assetLock(entry.second->mutex);

        if (entry.second->status == Status::Loaded && entry.second->refCount == 0)
        {
            entry.second->status = Status::Unloaded;
            entry.second->destructor(entry.second->data);
            entry.second->data = nullptr;
            CUBOS_DEBUG("Unloaded asset {}", core::data::Debug(AnyAsset(entry.first)));
        }
    }
}

void Assets::loadMeta(std::string_view path)
{
    auto file = core::data::FileSystem::find(path);
    if (file == nullptr)
    {
        CUBOS_ERROR("Couldn't load asset metadata: file '{}' not found", path);
        return;
    }

    if (file->isDirectory())
    {
        auto child = file->getChild();
        while (child != nullptr)
        {
            this->loadMeta(child->getPath());
            child = child->getSibling();
        }
    }
    else if (file->getName().ends_with(".meta"))
    {
        CUBOS_DEBUG("Loading asset metadata from '{}'", path);

        // Read the file contents into a string.
        std::string contents;
        {
            auto stream = file->open(core::data::File::OpenMode::Read);
            stream->readUntil(contents, nullptr);
        }

        // Deserialize the asset metadata from the JSON string.
        auto meta = AssetMeta();
        auto des = core::data::JSONDeserializer(contents);
        des.read(meta);
        if (des.failed())
        {
            CUBOS_ERROR("Couldn't load asset metadata: JSON deserialization failed for file '{}'", path);
            return;
        }

        // Check if the metadata has a path field, which is always ignored.
        if (meta.get("path").has_value())
        {
            CUBOS_WARN("Asset metadata at '{}' has a path field, which is always ignored, since it is derived from the "
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
            CUBOS_WARN("Asset metadata at '{}' has an unspecified/invalid UUID, generating a random one", path);
            id = uuids::uuid_random_generator(this->random.value())();
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

        CUBOS_DEBUG("Loaded asset {} metadata from '{}'", core::data::Debug(handle), path);
    }
}

AnyAsset Assets::load(AnyAsset handle) const
{
    auto assetEntry = this->entry(handle);
    if (assetEntry == nullptr)
    {
        CUBOS_ERROR("Could not load asset");
        return {};
    }

    // Find a bridge for the asset.
    auto bridge = this->bridge(handle);
    if (bridge == nullptr)
    {
        CUBOS_ERROR("Could not load asset");
        return {};
    }

    // We need to lock this to prevent the asset from being queued twice by a concurrent thread.
    // We also check if this is being called from the loader thread, in which case we don't need
    // to queue the asset.
    std::unique_lock lock(this->loaderMutex);
    if (assetEntry->status == Assets::Status::Unloaded && std::this_thread::get_id() != this->loaderThread.get_id())
    {
        CUBOS_TRACE("Queuing asset {} for loading", core::data::Debug(handle));
        assetEntry->status = Assets::Status::Loading;
        this->loaderQueue.push_back(Task{handle, bridge});
        this->loaderCond.notify_one();
    }
    lock.unlock();

    // Return a strong handle to the asset.
    assetEntry->refCount += 1;
    handle.refCount = &assetEntry->refCount;
    return handle;
}

bool Assets::saveMeta(const AnyAsset& handle) const
{
    // Get the asset metadata.
    auto meta = this->readMeta(handle);
    if (auto path = meta->get("path"))
    {
        CUBOS_DEBUG("Saving metadata for asset {}", core::data::Debug(handle));

        auto stream = core::data::FileSystem::open(*path + ".meta", core::data::File::OpenMode::Write);
        if (stream == nullptr)
        {
            CUBOS_ERROR("Could not save asset: could not open file '{}.meta'", *path);
            return false;
        }

        auto serializer = core::data::JSONSerializer(*stream, 4);
        serializer.context().push(AssetMeta::Exclude{{"path"}}); // Don't save the path field.
        serializer.write(*meta, nullptr);
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
    CUBOS_DEBUG("Saving data for asset {}", core::data::Debug(handle));
    return bridge->save(*this, handle);
}

Assets::Status Assets::status(const AnyAsset& handle) const
{
    std::shared_lock lock(this->mutex);

    // Do not use .entry() here because we don't want to log errors if the asset is unknown.
    auto it = this->entries.find(handle.getId());
    if (it == this->entries.end())
    {
        return Status::Unknown;
    }

    return it->second->status;
}

bool Assets::update(AnyAsset& handle)
{
    auto assetEntry = this->entry(handle);
    if (assetEntry == nullptr)
    {
        CUBOS_ERROR("Could not update asset handle");
        return false;
    }

    if (assetEntry->version > handle.getVersion())
    {
        handle.version = assetEntry->version;
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
                     assetEntry->type.name());
        assetEntry->destructor(assetEntry->data);
        assetEntry->data = nullptr;
        assetEntry->status = Status::Unloaded;
        assetEntry->version++;

        CUBOS_DEBUG("Invalidated asset {}", core::data::Debug(handle));
    }
}

AssetMetaRead Assets::readMeta(const AnyAsset& handle) const
{
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
    : status(Status::Unloaded)
    , refCount(0)
    , version(0)
    , data(nullptr)
    , type(typeid(void)) // NOLINT(modernize-redundant-void-arg)
{
}

AnyAsset Assets::create(std::type_index type, void* data, void (*destructor)(void*))
{
    // Generate a new UUID and store the asset.
    auto id = uuids::uuid_random_generator(this->random.value())();
    return this->store(AnyAsset(id), type, data, destructor);
}

AnyAsset Assets::store(AnyAsset handle, std::type_index type, void* data, void (*destructor)(void*))
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
                     assetEntry->type.name());
        assetEntry->destructor(assetEntry->data);
    }

    // Mark it as loaded and set its data.
    assetEntry->status = Status::Loaded;
    assetEntry->version++;
    assetEntry->data = data;
    assetEntry->type = type;
    assetEntry->destructor = destructor;
    assetEntry->cond.notify_all();

    CUBOS_DEBUG("Stored data of type {} for asset {}", type.name(), core::data::Debug(handle));

    // Return a strong handle to the asset.
    assetEntry->refCount.fetch_add(1);
    handle.refCount = &assetEntry->refCount;
    handle.version = assetEntry->version;
    return handle;
}

template <typename Lock>
void* Assets::access(const AnyAsset& handle, std::type_index type, Lock& lock, bool incVersion) const
{
    CUBOS_ASSERT(!handle.isNull(), "Could not access asset");

    // Get the entry for the asset.
    auto assetEntry = this->entry(handle);
    CUBOS_ASSERT(assetEntry != nullptr, "Could not access asset");

    // If this is being called from the loader thread, we should load the asset synchronously.
    if (std::this_thread::get_id() == this->loaderThread.get_id() && assetEntry->status != Status::Loaded)
    {
        CUBOS_DEBUG("Loading asset {} as a dependency", core::data::Debug(handle));

        auto bridge = this->bridge(handle);
        CUBOS_ASSERT(bridge != nullptr, "Could not access asset");

        // Load the asset - the const_cast is okay since the const qualifiers are only used to make
        // the interface more readable. We need to unlock temporarily to avoid a deadlock, since the
        // bridge will call back into the asset manager.
        lock.unlock();
        if (!bridge->load(const_cast<Assets&>(*this), handle))
        {
            CUBOS_CRITICAL("Could not load asset {}", core::data::Debug(handle));
            abort();
        }
        lock.lock();
    }

    // Wait until the asset finishes loading.
    while (assetEntry->status == Status::Loading)
    {
        assetEntry->cond.wait(lock);
    }

    CUBOS_ASSERT(assetEntry->status == Status::Loaded && assetEntry->data != nullptr, "Could not access asset");
    CUBOS_ASSERT(assetEntry->type == type, "Type mismatch, expected {}, got {}", type.name(), assetEntry->type.name());

    if (incVersion)
    {
        assetEntry->version++;
        CUBOS_DEBUG("Incremented version of asset {}", core::data::Debug(handle));
    }

    return assetEntry->data;
}

// Define the explicit instantiations of the access() function, one for each lock type.
// Necessary because the function is template - would otherwise cause linker errors.

template void* Assets::access<std::shared_lock<std::shared_mutex>>(const AnyAsset&, std::type_index,
                                                                   std::shared_lock<std::shared_mutex>&, bool) const;
template void* Assets::access<std::unique_lock<std::shared_mutex>>(const AnyAsset&, std::type_index,
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

std::shared_ptr<Assets::Entry> Assets::entry(const AnyAsset& handle) const
{
    // If the handle is null, we can't access the asset.
    if (handle.isNull())
    {
        CUBOS_ERROR("Null asset handle");
        return nullptr;
    }

    // Lock the entries map for reading.
    auto sharedLock = std::shared_lock(this->mutex);

    // Search for the entry in the map.
    auto it = this->entries.find(handle.getId());
    if (it == this->entries.end())
    {
        CUBOS_ERROR("No such asset {}", core::data::Debug(handle));
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
    auto uniqueLock = std::unique_lock(this->mutex, std::defer_lock);
    auto sharedLock = std::shared_lock(this->mutex, std::defer_lock);
    if (create)
    {
        uniqueLock.lock();
    }
    else
    {
        sharedLock.lock();
    }

    // Search for an existing entry for the asset.
    auto it = this->entries.find(handle.getId());
    if (it == this->entries.end())
    {
        // If we're creating the asset, create a new entry for it.
        // Otherwise, return nullptr.
        if (create)
        {
            auto entry = std::make_shared<Entry>();
            entry->meta.set("id", uuids::to_string(handle.getId()));
            it = this->entries.emplace(handle.getId(), std::move(entry)).first;
            CUBOS_TRACE("Created new asset entry for {}", core::data::Debug(handle));
        }
        else
        {
            CUBOS_ERROR("No such asset {}", core::data::Debug(handle));
            return nullptr;
        }
    }

    return it->second;
}

std::shared_ptr<AssetBridge> Assets::bridge(const AnyAsset& handle) const
{
    auto meta = this->readMeta(handle);
    if (auto path = meta->get("path"))
    {
        std::shared_lock lock(this->mutex);

        // Get the bridge which has the best match for this asset.
        std::shared_ptr<AssetBridge> best = nullptr;
        std::size_t bestLen = 0;
        for (const auto& bridge : this->bridges)
        {
            if (path->ends_with(bridge.first) && bridge.first.length() > bestLen)
            {
                best = bridge.second;
                bestLen = bridge.first.length();
            }
        }

        if (best == nullptr)
        {
            CUBOS_ERROR("Cannot find a matching bridge for asset path {}", *path);
        }

        return best;
    }

    CUBOS_ERROR("Cannot find a bridge for asset {}: asset does not have a path", core::data::Debug(handle));
    return nullptr;
}

void Assets::loader()
{
    for (;;)
    {
        // Wait for a new asset to load.
        std::unique_lock<std::mutex> loaderLock(this->loaderMutex);
        this->loaderCond.wait(loaderLock, [this]() { return !this->loaderQueue.empty() || this->loaderShouldExit; });

        // If the loader thread should exit, exit.
        if (this->loaderShouldExit)
        {
            return;
        }

        // Get the next asset to load.
        auto task = this->loaderQueue.front();
        this->loaderQueue.pop_front();
        loaderLock.unlock(); // Unlock the mutex before loading the asset.

        if (!task.bridge->load(*this, task.handle))
        {
            CUBOS_ERROR("Failed to load asset '{}'", core::data::Debug(task.handle));

            auto assetEntry = this->entry(task.handle);
            CUBOS_ASSERT(assetEntry != nullptr, "This should never happen");
            assetEntry->status = Assets::Status::Unloaded;
        }
    }
}
