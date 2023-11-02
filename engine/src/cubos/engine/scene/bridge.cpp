#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/old/json_deserializer.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::old::JSONDeserializer;
using cubos::core::data::old::SerializationMap;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityHash;
using cubos::core::ecs::Registry;
using cubos::core::memory::Stream;

using namespace cubos::engine;

bool SceneBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    // Dump the file contents into a string.
    std::string contents;
    stream.readUntil(contents, nullptr);

    // Deserialize the scene file.
    auto deserializer = JSONDeserializer(contents);
    if (deserializer.failed())
    {
        CUBOS_ERROR("Could not parse scene file as JSON");
        return false;
    }

    auto scene = Scene();

    // Add a SerializationMap for entity handle deserialization.
    deserializer.context().push(
        SerializationMap<Entity, std::string, EntityHash>{[&](const Entity&, std::string&) {
                                                              return false; // Serialization not needed.
                                                          },
                                                          [&](Entity& entity, const std::string& string) {
                                                              if (!scene.blueprint.bimap().containsRight(string))
                                                              {
                                                                  return false;
                                                              }

                                                              entity = scene.blueprint.bimap().atLeft(string);
                                                              return true;
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

        Entity entity{};
        if (scene.blueprint.bimap().containsRight(name))
        {
            entity = scene.blueprint.bimap().atLeft(name);
        }

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
                CUBOS_ERROR("Entity '{}' not found in scene", name);
                return false;
            }
        }

        // Read the components of the entity.
        std::size_t componentsLen = deserializer.beginDictionary();
        for (std::size_t j = 0; j < componentsLen; ++j)
        {
            std::string componentName{};
            deserializer.read(componentName);

            if (!Registry::create(componentName, deserializer, scene.blueprint, entity))
            {
                CUBOS_ERROR("Could not deserialize component '{}' into entity '{}' in scene", componentName, name);
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

bool SceneBridge::saveToFile(const Assets& /*assets*/, const AnyAsset& /*handle*/, Stream& /*stream*/)
{
    // TODO: figure out how to do this.
    // One of the problems is finding out exactly what was overriden in sub-scenes.
    // One way to solve it would be to have a way to compare entities between blueprints, maybe
    // with a hash?
    CUBOS_CRITICAL("SceneBridge::save not implemented");
    return false;
}
