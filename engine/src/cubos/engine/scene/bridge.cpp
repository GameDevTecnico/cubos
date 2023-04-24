#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/json_deserializer.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using namespace cubos::engine;
using namespace cubos::core;

bool SceneBridge::load(Assets& assets, AnyAsset handle)
{
    // Open the scene file.
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = data::FileSystem::open(path, data::File::OpenMode::Read);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open scene file '{}'", path);
        return false;
    }

    // Dump the file contents into a string.
    std::string contents;
    stream->readUntil(contents, nullptr);
    stream.reset(); // Close the file.

    // Deserialize the scene file.
    auto deserializer = data::JSONDeserializer(contents);
    if (deserializer.failed())
    {
        CUBOS_ERROR("Could not parse scene file '{}' as JSON", path);
        return false;
    }

    auto scene = Scene();
    deserializer.beginObject();

    // First, read the imports section.
    size_t len = deserializer.beginDictionary();
    for (size_t i = 0; i < len; ++i)
    {
        // Read the import name and asset ID.
        std::string name, id;
        deserializer.read(name);
        deserializer.read(id);

        // Check if there's already an import with that name.
        if (scene.imports.find(name) != scene.imports.end())
        {
            CUBOS_ERROR("Scenes cannot have two imports with the same name");
            return false;
        }

        // Load the imported scene.
        auto importedHandle = Asset<Scene>(id);
        if (importedHandle.getId() == handle.getId())
        {
            CUBOS_ERROR("Scenes cannot import themselves");
            return false;
        }
        auto imported = assets.read(importedHandle);

        // Add the imported scene to the scene.
        scene.imports[name] = importedHandle;
        scene.blueprint.merge(name, imported->blueprint);
    }
    deserializer.endDictionary();

    // Then, read the entities section. Here, we may find entities that have already been added
    // by the imports section, in which case we'll just update them.
    len = deserializer.beginDictionary();
    for (size_t i = 0; i < len; ++i)
    {
        // Get the name of the entity and check if its valid or already in the blueprint.
        std::string name;
        deserializer.read(name);
        auto entity = scene.blueprint.entity(name);

        if (entity.isNull())
        {
            // Then, either the entity must be created, or, if the entity name has a dot, an error
            // should be printed since the entity was not found.
            if (name.find('.') == std::string::npos)
            {
                entity = scene.blueprint.create(name);
            }
            else
            {
                CUBOS_ERROR("Entity '{}' not found in scene '{}'", name, path);
                return false;
            }
        }

        // Read the components of the entity.
        size_t componentsLen = deserializer.beginDictionary();
        for (size_t j = 0; j < componentsLen; ++j)
        {
            std::string componentName;
            deserializer.read(componentName);
            if (!scene.blueprint.addFromDeserializer(entity, componentName, deserializer))
            {
                CUBOS_ERROR("Could not deserialize component '{}' into entity '{}' in scene '{}'", componentName, name,
                            path);
                return false;
            }
        }
        deserializer.endDictionary();
    }
    deserializer.endDictionary();

    deserializer.endObject();

    // Finally, write the scene to the asset.
    assets.store(handle, std::move(scene));
    return true;
}

bool SceneBridge::save(const Assets&, AnyAsset)
{
    // TODO: figure out how to do this.
    // One of the problems is finding out exactly what was overriden in sub-scenes.
    // One way to solve it would be to have a way to compare entities between blueprints, maybe
    // with a hash?
    CUBOS_CRITICAL("SceneBridge::save not implemented");
    return false;
}
