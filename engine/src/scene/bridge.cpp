#include <cubos/core/data/des/json.hpp>
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/bridge.hpp>
#include <cubos/engine/scene/scene.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::data::JSONSerializer;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::Entity;
using cubos::core::memory::AnyValue;
using cubos::core::memory::Stream;
using cubos::core::reflection::TypeRegistry;

using namespace cubos::engine;

SceneBridge::SceneBridge(TypeRegistry components, TypeRegistry relations)
    : FileBridge(core::reflection::reflect<Scene>())
    , mComponents{std::move(components)}
    , mRelations{std::move(relations)}
{
}

TypeRegistry& SceneBridge::components()
{
    return mComponents;
}

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
                CUBOS_ERROR("Expected entity {} to be a JSON object, found {}", entityName, entitiesJSON.type_name());
                return false;
            }

            auto entity = scene.blueprint.bimap().atRight(entityName);

            for (const auto& [typeName, dataJSON] : entityJSON.items())
            {
                if (mComponents.contains(typeName))
                {
                    // Create the component with the default value.
                    auto component = AnyValue::defaultConstruct(mComponents.at(typeName));

                    // Then change any values using the associated JSON.
                    des.feed(dataJSON);
                    if (!des.read(component.type(), component.get()))
                    {
                        CUBOS_ERROR("Could not deserialize component of type {} of entity {}", typeName, entityName);
                        return false;
                    }

                    // And finally, add it to the blueprint.
                    scene.blueprint.add(entity, core::memory::move(component));
                }
                else if (mRelations.contains(typeName))
                {
                    // Create the relation with the default value.
                    auto relation = AnyValue::defaultConstruct(mRelations.at(typeName));

                    std::string toName;
                    if (dataJSON.is_string())
                    {
                        // If the user only specified a string, then the relation will keep the default value and the
                        // string represents the 'to' entity.
                        toName = dataJSON;
                    }
                    else if (dataJSON.is_object())
                    {
                        // If the user specified an object, then it must contain an "entity" key, whose value
                        // represents the 'to' entity.
                        if (!dataJSON.contains("entity") || !dataJSON.at("entity").is_string())
                        {
                            CUBOS_ERROR("Could not deserialize relation of type {} from entity {}: expected JSON "
                                        "object to contain key \"entity\" with a string value",
                                        typeName, entityName, dataJSON.type_name());
                            return false;
                        }
                        toName = dataJSON.at("entity");

                        // If the value is specified, then deserialize the relation from it.
                        if (dataJSON.contains("value"))
                        {
                            des.feed(dataJSON.at("value"));
                            if (!des.read(relation.type(), relation.get()))
                            {
                                CUBOS_ERROR("Could not deserialize relation of type {} from entity {} to {}", typeName,
                                            entityName, toName);
                                return false;
                            }
                        }

                        // Check if there extra unused fields in the object.
                        for (const auto& [key, unusedJSON] : dataJSON.items())
                        {
                            if (key != "entity" && key != "value")
                            {
                                CUBOS_ERROR("Could not deserialize relation of type {} from entity {} to entity {}: "
                                            "expected \"entity\" or \"value\", found {}",
                                            typeName, entityName, toName, key);
                                return false;
                            }
                        }
                    }
                    else
                    {
                        CUBOS_ERROR("Could not deserialize relation of type {} from entity {}: expected JSON object "
                                    "but found {}",
                                    typeName, entityName, dataJSON.type_name());
                        return false;
                    }

                    if (!scene.blueprint.bimap().containsRight(toName))
                    {
                        CUBOS_ERROR("Could not deserialize relation of type {} from entity {} to {}: no such entity {}",
                                    typeName, entityName, toName, toName);
                        return false;
                    }

                    auto toEntity = scene.blueprint.bimap().atRight(toName);
                    scene.blueprint.relate(entity, toEntity, std::move(relation));
                }
                else
                {
                    CUBOS_ERROR("No such component or relation type {} registered on the scene bridge", typeName);
                    return false;
                }
            }
        }
    }

    // Finally, write the scene to the asset.
    assets.store(handle, std::move(scene));
    return true;
}

bool SceneBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto scene = assets.read<Scene>(handle);
    JSONSerializer ser{};
    auto json = nlohmann::json::object();

    auto importJson = nlohmann::json::object();
    for (const auto& [name, subHandle] : scene->imports)
    {
        importJson.emplace(name.c_str(), uuids::to_string(subHandle.getId()));
    }
    json.push_back({"imports", importJson});

    auto entitiesJson = nlohmann::json::object();
    for (const auto& [entity, name] : scene->blueprint.GetEntities())
    {
        if (name.find(".") == std::string::npos)
        {
            auto entityJson = nlohmann::json::object();
            for (auto& [type, components] : scene->blueprint.GetComponents())
            {
                if (!components.contains(entity))
                {
                    continue;
                }
                ser.write(components[entity].type(), components[entity].get());
                entityJson.emplace(type->name().c_str(), ser.output());
            }

            entitiesJson.emplace(name.c_str(), entityJson);
        }
    }
    json.push_back({"entities", entitiesJson});

    stream.print(json.dump(2));
    return true;
}
