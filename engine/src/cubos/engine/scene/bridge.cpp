#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/old/json_deserializer.hpp>
#include <cubos/core/ecs/entity/hash.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::old::JSONDeserializer;
using cubos::core::data::old::SerializationMap;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityHash;

using namespace cubos::engine;

bool SceneBridge::load(Assets& assets, const AnyAsset& handle)
{
    // Open the scene file.
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Read);
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
    auto deserializer = JSONDeserializer(contents);
    if (deserializer.failed())
    {
        CUBOS_ERROR("Could not parse scene file '{}' as JSON", path);
        return false;
    }

    auto scene = Scene();

    // Add a SerializationMap for entity handle deserialization.
    deserializer.context().push(
        SerializationMap<Entity, std::string, EntityHash>{[&](const Entity&, std::string&) {
                                                              return false; // Serialization not needed.
                                                          },
                                                          [&](Entity& entity, const std::string& string) {
                                                              entity = scene.blueprint.entity(string);
                                                              return !entity.isNull();
                                                          }});

    deserializer.beginObject();

    // First, read the imports section.
    std::size_t len = deserializer.beginDictionary();
    for (std::size_t i = 0; i < len; ++i)
    {
        // Read the import name and asset ID.
        std::string name;
        std::string id;
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
    for (std::size_t i = 0; i < len; ++i)
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
        std::size_t componentsLen = deserializer.beginDictionary();
        for (std::size_t j = 0; j < componentsLen; ++j)
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

bool SceneBridge::save(const Assets& /*assets*/, const AnyAsset& /*handle*/)
{
    // TODO: figure out how to do this.
    // One of the problems is finding out exactly what was overriden in sub-scenes.
    // One way to solve it would be to have a way to compare entities between blueprints, maybe
    // with a hash?
    CUBOS_CRITICAL("SceneBridge::save not implemented");
    return false;
}
