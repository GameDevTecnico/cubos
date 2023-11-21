#include <cubos/core/data/des/json.hpp>
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Registry;
using cubos::core::memory::AnyValue;
using cubos::core::memory::Stream;

using namespace cubos::engine;

bool SceneBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    // Dump the file contents into a string.
    std::string contents{};
    stream.readUntil(contents, nullptr);

    // Parse the file contents as JSON.
    nlohmann::json json{};

    try
    {
        json = nlohmann::json::parse(contents);
    }
    catch (nlohmann::json::parse_error& e)
    {
        CUBOS_ERROR("{}", e.what());
        return false;
    }

    // Check if the parsed JSON follows the expected scene structure
    if (!json.is_object())
    {
        CUBOS_ERROR("Expected root to be a JSON object, found {}", json.type_name());
        return false;
    }

    for (const auto& [name, item] : json.items())
    {
        if (name != "imports" && name != "entities")
        {
            CUBOS_ERROR("Expected 'imports' or 'entities', found '{}'", name);
            return false;
        }
    }

    Scene scene{};
    JSONDeserializer des{};
    des.hook<Entity>([&des, &scene](Entity& entity) {
        std::string name{};
        if (!des.read(name))
        {
            return false;
        }

        if (!scene.blueprint.bimap().containsRight(name))
        {
            CUBOS_ERROR("Could not deserialize entity from name, no such entity '{}' in scene", name);
            return false;
        }

        entity = scene.blueprint.bimap().atRight(name);
        return true;
    });

    if (json.contains("imports"))
    {
        des.feed(json.at("imports"));
        if (!des.read(scene.imports))
        {
            CUBOS_ERROR("Could not deserialize scene imports");
            return false;
        }

        for (const auto& [name, asset] : scene.imports)
        {
            scene.blueprint.merge(name, assets.read(asset)->blueprint);
        }
    }

    if (json.contains("entities"))
    {
        const auto& entitiesJSON = json.at("entities");
        if (!entitiesJSON.is_object())
        {
            CUBOS_ERROR("Expected 'entities' to be a JSON object, found {}", entitiesJSON.type_name());
            return false;
        }

        // Create all entities first.
        for (const auto& [entityName, entityJSON] : entitiesJSON.items())
        {
            // If we didn't find an entity with the given name, then we must create one.
            if (!scene.blueprint.bimap().containsRight(entityName))
            {
                // If the entity name contains a dot, then it must have been imported. If we didn't
                // find it, the user did something wrong.
                if (entityName.find('.') != std::string::npos)
                {
                    CUBOS_ERROR("No such entity '{}' was imported", entityName);
                    return false;
                }

                if (!Blueprint::validEntityName(entityName))
                {
                    CUBOS_ERROR("Invalid entity name '{}'", entityName);
                    return false;
                }

                scene.blueprint.create(entityName);
            }
        }

        // Then add their components. This way the Entity hook will be able to handle entities which
        // are declared after it in the scene file.
        for (const auto& [entityName, entityJSON] : entitiesJSON.items())
        {
            if (!entityJSON.is_object())
            {
                CUBOS_ERROR("Expected entity '{}' to be a JSON object, found {}", entityName, entitiesJSON.type_name());
                return false;
            }

            auto entity = scene.blueprint.bimap().atRight(entityName);

            for (const auto& [componentName, componentJSON] : entityJSON.items())
            {
                const auto* type = Registry::type(componentName);
                if (type == nullptr)
                {
                    CUBOS_ERROR("No such component type '{}'", componentName);
                    return false;
                }

                auto component = AnyValue::defaultConstruct(*type);
                des.feed(componentJSON);
                if (!des.read(component.type(), component.get()))
                {
                    CUBOS_ERROR("Could not deserialize component of type '{}' of entity '{}'", componentName,
                                entityName);
                    return false;
                }

                scene.blueprint.add(entity, core::memory::move(component));
            }
        }
    }

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
