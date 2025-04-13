#include <cubos/core/data/des/json.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/transform/child_of.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::Entity;
using cubos::core::memory::AnyValue;
using cubos::core::reflection::ConstructibleTrait;
using cubos::engine::Asset;
using cubos::engine::Scene;
using cubos::engine::SceneNode;

CUBOS_REFLECT_IMPL(Scene)
{
    return core::reflection::Type::create("cubos::engine::Scene")
        .with(ConstructibleTrait::typed<Scene>().withDefaultConstructor().withMoveConstructor().build());
}

bool Scene::loadFromNode(SceneNode root, const Assets& assets)
{
    mRoot = std::move(root);
    mBlueprint.clear();
    this->loadEntities(mRoot, assets, "");
    return this->loadComponentsAndRelations(mRoot, "");
}

const SceneNode& Scene::node() const
{
    return mRoot;
}

const Blueprint& Scene::blueprint() const
{
    return mBlueprint;
}

Entity Scene::loadEntities(const SceneNode& node, const Assets& assets, std::string name)
{
    Entity entity;

    if (!node.inherits().isNull())
    {
        assets.read(node.inherits())
            ->blueprint()
            .instantiate(
                [&](const std::string& inheritsName) -> Entity {
                    // Replace everything before the first # with the current name.
                    auto i = inheritsName.find_first_of('#');
                    auto finalName = i == std::string::npos ? name : (name + inheritsName.substr(i));
                    auto entity = mBlueprint.create(finalName);
                    mEntityMap[finalName] = entity;
                    return entity;
                },
                [&](Entity entity, AnyValue component) { mBlueprint.add(entity, std::move(component)); },
                [&](Entity fromEntity, Entity toEntity, AnyValue relation) {
                    mBlueprint.relate(fromEntity, toEntity, std::move(relation));
                });
        entity = mEntityMap[name];
    }
    else if (mBlueprint.bimap().containsRight(name))
    {
        entity = mBlueprint.bimap().atRight(name);
        mEntityMap[name] = entity;
    }
    else
    {
        entity = mBlueprint.create(name);
        mEntityMap[name] = entity;
    }

    for (const auto& [childName, childNode] : node.children())
    {
        std::string childFullName = name;
        childFullName.push_back('#');
        childFullName.append(childName);
        Entity childEntity = this->loadEntities(*childNode, assets, std::move(childFullName));
        mBlueprint.relate(childEntity, entity, ChildOf{});
    }

    return entity;
}

bool Scene::loadComponentsAndRelations(const SceneNode& node, const std::string& name)
{
    JSONDeserializer des{};

    // We set up a deserializer hook to handle the deserialization of entities.
    // We need to translate the entity name to the actual entity we've created in loadEntities.
    des.hook<Entity>([&des, this](Entity& entity) {
        std::string name{};
        if (!des.read(name))
        {
            return false;
        }

        if (name == "null")
        {
            entity = {};
        }
        else
        {
            if (!mEntityMap.contains(name))
            {
                CUBOS_ERROR("Entity {} wasn't found", name);
                return false;
            }

            entity = mEntityMap[name];
        }

        return true;
    });

    // Fetch the name of the entity corresponding to this node, which we created in loadEntities.
    Entity entity = mEntityMap[name];

    // Deserialize each of the components in the current node, and add them to the blueprint.
    for (const auto& [type, json] : node.components())
    {
        auto component = AnyValue::defaultConstruct(*type);
        des.feed(json);
        if (!des.read(component.type(), component.get()))
        {
            CUBOS_ERROR("Failed to deserialize component of type {}", type->name());
            return false;
        }
        mBlueprint.add(entity, component);
    }

    // Deserialize each of the relations in the current node, and add them to the blueprint.
    // We need to translate the entity name to the actual entity we've created in loadEntities.
    for (const auto& [type, entityMap] : node.relationsFrom())
    {
        for (const auto& [fromName, json] : entityMap)
        {
            auto relation = AnyValue::defaultConstruct(*type);
            des.feed(json);
            if (!des.read(relation.type(), relation.get()))
            {
                CUBOS_ERROR("Failed to deserialize relation of type {} from entity {} to {}", type->name(), fromName,
                            name.empty() ? "root" : ("entity " + name));
                return false;
            }

            if (!mEntityMap.contains(fromName))
            {
                CUBOS_ERROR("Source entity {} for relation of type {} with target {} wasn't found", fromName,
                            type->name(), name.empty() ? "root" : ("entity " + name));
                return false;
            }

            mBlueprint.relate(mEntityMap[fromName], entity, relation);
        }
    }
    for (const auto& [type, entityMap] : node.relationsTo())
    {
        for (const auto& [toName, json] : entityMap)
        {
            auto relation = AnyValue::defaultConstruct(*type);
            des.feed(json);
            if (!des.read(relation.type(), relation.get()))
            {
                CUBOS_ERROR("Failed to deserialize relation of type {} from entity {} to {}", type->name(), name,
                            toName.empty() ? "root" : ("entity " + toName));
                return false;
            }

            if (!mEntityMap.contains(toName))
            {
                CUBOS_ERROR("Target entity {} for relation of type {} with source {} wasn't found", toName,
                            type->name(), name.empty() ? "root" : ("entity " + name));
                return false;
            }

            mBlueprint.relate(entity, mEntityMap[toName], relation);
        }
    }

    // Apply the same process to all children of the current node.
    for (const auto& [childName, childNode] : node.children())
    {
        std::string childFullName = name;
        childFullName.push_back('#');
        childFullName.append(childName);
        if (!this->loadComponentsAndRelations(*childNode, childFullName))
        {
            return false;
        }
    }

    return true;
}
