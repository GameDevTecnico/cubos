#include <cubos/engine/data/scene.hpp>
#include <cubos/engine/data/asset_manager.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/json_deserializer.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos;
using namespace cubos::engine::data;

impl::SceneLoader::SceneLoader(AssetManager* manager) : Loader(manager)
{
    // Do nothing.
}

const void* impl::SceneLoader::load(const Meta& meta)
{
    auto path = meta.getParameters().find("path");
    if (path == meta.getParameters().end())
    {
        CUBOS_ERROR("No path specified for scene file");
        return nullptr;
    }

    auto file = core::data::FileSystem::find(path->second);
    if (!file)
    {
        CUBOS_ERROR("Could not find scene file '{}'", path->second);
        return nullptr;
    }

    auto stream = file->open(core::data::File::OpenMode::Read);
    if (!stream)
    {
        CUBOS_ERROR("Could not open scene file '{}'", path->second);
        return nullptr;
    }

    std::string src;
    stream->readUntil(src, nullptr); // Read the whole file.
    auto deserializer = core::data::JSONDeserializer(src);
    if (deserializer.failed())
    {
        CUBOS_ERROR("Could not parse scene file '{}' as JSON", path->second);
        return nullptr;
    }

    auto asset = new Scene();

    // Prepare handle context to deserialize assets.
    auto handleCtx = [&](core::data::Deserializer& des, core::data::Handle& handle) {
        std::string id;
        des.read(id);
        handle = this->manager->loadAny(id);
    };

    deserializer.beginObject();

    // Read imports sections.
    auto len = deserializer.beginDictionary();
    for (auto i = 0; i < len; ++i)
    {
        std::string name, path;
        deserializer.readString(name);
        deserializer.readString(path);
        asset->blueprint.merge(name, this->manager->load<Scene>(path)->blueprint);
    }
    deserializer.endDictionary();

    // Read entities section.
    len = deserializer.beginDictionary();
    for (auto i = 0; i < len; ++i)
    {
        // Get the name of the entity and check if its valid or already in the blueprint.
        std::string name;
        deserializer.read(name);
        auto entity = asset->blueprint.entity(name);

        // Check if the entity doesn't exist and needs to be created.
        if (entity.isNull())
        {
            if (name.find(".") == std::string::npos)
            {
                // Local entity, can create.
                entity = asset->blueprint.create(name);
            }
            else
            {
                // Entity isn't local so log an error.
                CUBOS_ERROR("Entity '{}' does not exist", name);
                delete asset;
                return nullptr;
            }
        }

        // Get the components of the entity.
        auto compCount = deserializer.beginDictionary();
        for (auto j = 0; j < compCount; ++j)
        {
            std::string compName;
            deserializer.readString(compName);
            if (!asset->blueprint.addFromDeserializer(entity, compName, deserializer, handleCtx))
            {
                CUBOS_ERROR("Could not add component '{}' to entity '{}'", compName, name);
                delete asset;
                return nullptr;
            }
        }
        deserializer.endDictionary();
    }

    deserializer.endDictionary();
    deserializer.endObject();

    return asset;
}

std::future<const void*> impl::SceneLoader::loadAsync(const Meta& meta)
{
    return std::async(std::launch::async, [this, &meta] { return load(meta); });
}

void impl::SceneLoader::unload(const Meta& meta, const void* asset)
{
    delete static_cast<const Scene*>(asset);
}
