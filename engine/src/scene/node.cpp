#include <cubos/engine/scene/node.hpp>

using cubos::core::ecs::Blueprint;
using cubos::core::memory::AnyValue;
using cubos::core::memory::TypeMap;
using cubos::core::reflection::Type;

using cubos::engine::Asset;
using cubos::engine::Entity;
using cubos::engine::Scene;
using cubos::engine::SceneNode;

Asset<Scene> SceneNode::inherits() const
{
    return mInherits;
}

void SceneNode::inherits(Asset<Scene> scene)
{
    mInherits = std::move(scene);
}

const TypeMap<nlohmann::json>& SceneNode::components() const
{
    return mComponents;
}

void SceneNode::add(const Type& type, nlohmann::json data)
{
    mComponents.insert(type, std::move(data));
}

void SceneNode::remove(const Type& type)
{
    mComponents.erase(type);
}

const TypeMap<std::unordered_map<std::string, nlohmann::json>>& SceneNode::relationsFrom() const
{
    return mRelationsFrom;
}

const TypeMap<std::unordered_map<std::string, nlohmann::json>>& SceneNode::relationsTo() const
{
    return mRelationsTo;
}

void SceneNode::relateFrom(const Type& type, const std::string& entity, nlohmann::json data)
{
    if (!mRelationsFrom.contains(type))
    {
        mRelationsFrom.insert(type, {});
    }

    mRelationsFrom.at(type).insert_or_assign(entity, std::move(data));
}

void SceneNode::relateTo(const Type& type, const std::string& entity, nlohmann::json data)
{
    if (!mRelationsTo.contains(type))
    {
        mRelationsTo.insert(type, {});
    }

    mRelationsTo.at(type).insert_or_assign(entity, std::move(data));
}

void SceneNode::unrelateFrom(const Type& type, const std::string& entity)
{
    if (mRelationsFrom.contains(type))
    {
        mRelationsFrom.at(type).erase(entity);
    }
}

void SceneNode::unrelateTo(const Type& type, const std::string& entity)
{
    if (mRelationsTo.contains(type))
    {
        mRelationsTo.at(type).erase(entity);
    }
}

const std::unordered_map<std::string, std::unique_ptr<SceneNode>>& SceneNode::children() const
{
    return mChildren;
}

SceneNode& SceneNode::create(const std::string& name)
{
    if (!mChildren.contains(name))
    {
        mChildren.emplace(name, std::make_unique<SceneNode>());
    }

    return *mChildren.at(name);
}

void SceneNode::destroy(const std::string& name)
{
    mChildren.erase(name);
}

bool SceneNode::load(const nlohmann::json& json, const core::reflection::TypeRegistry& components,
                     const core::reflection::TypeRegistry& relations)
{
    if (!json.is_object())
    {
        CUBOS_ERROR("Expected scene node to be an object, got {} instead", json.type_name());
        return false;
    }

    for (const auto& [key, value] : json.items())
    {
        if (key == "inherit")
        {
            const auto& inherit = json.at("inherit");
            if (!inherit.is_string())
            {
                CUBOS_ERROR("Expected inherit to be a string, got {} instead", inherit.type_name());
                return false;
            }
            this->inherits(Asset<Scene>{inherit.get<std::string>()});
        }
        else if (key.starts_with('#'))
        {
            // A child node.
            auto name = key.substr(1);
            if (!Blueprint::validEntityName(name))
            {
                CUBOS_ERROR(
                    "Invalid child node name {}, must contain only lowercase alphanumeric characters and hyphens",
                    name);
                return false;
            }
            if (name.find('#') != std::string::npos)
            {
                CUBOS_ERROR("Invalid child node name {}, must not contain more than one # character", name);
                return false;
            }

            if (!this->create(name).load(value, components, relations))
            {
                CUBOS_ERROR("Failed to load child node {}", name);
                return false;
            }
        }
        else if (key.find('#') != std::string::npos)
        {
            // A relation.
            auto i = key.find_first_of('#');
            auto typeName = key.substr(0, i);
            auto entity = key.substr(i + 1);

            if (!entity.empty())
            {
                // Entity "" is identified by "#", specially. But all other entities should preserve their #.
                entity.insert(entity.begin(), '#');
            }

            if (!relations.contains(typeName))
            {
                CUBOS_ERROR("Unknown relation type {}", typeName);
                return false;
            }

            this->relateTo(relations.at(typeName), entity, value);
        }
        else
        {
            // A component.
            if (!components.contains(key))
            {
                CUBOS_ERROR("Unknown component type {}", key);
                return false;
            }

            this->add(components.at(key), value);
        }
    }

    return true;
}
