/// @file
/// @brief Class @ref cubos::engine::SceneNode.
/// @ingroup scene-plugin

#pragma once

#include <memory>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type_registry.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    class Scene;

    /// @brief Describes a node in a scene hierarchy.
    ///
    /// A scene node represents an entity, its components and relations, and its children entities.
    /// Optionally, a node may inherit data (components, relations and children) from a root scene node of another
    /// scene asset.
    class CUBOS_ENGINE_API SceneNode
    {
    public:
        CUBOS_REFLECT;

        /// @brief Empty constructor.
        SceneNode() = default;

        /// @brief Move constructor.
        SceneNode(SceneNode&&) = default;

        /// @brief Move assignment.
        SceneNode& operator=(SceneNode&&) = default;

        /// @brief Gets the scene asset from which this node inherits.
        /// @return Scene asset.
        Asset<Scene> inherits() const;

        /// @brief Sets the scene asset from which this node inherits.
        /// @param scene Scene asset.
        void inherits(Asset<Scene> scene);

        /// @brief Returns the components of the entity.
        /// @return Components.
        const core::memory::TypeMap<nlohmann::json>& components() const;

        /// @brief Inserts a component of the given type into the node.
        /// @param type Component type.
        /// @param data Component data.
        void add(const core::reflection::Type& type, nlohmann::json data);

        /// @brief Removes the component of the given type from the node.
        /// @param type Component type.
        void remove(const core::reflection::Type& type);

        /// @brief Gets all incoming relations.
        /// @return Incoming relations.
        const core::memory::TypeMap<std::unordered_map<std::string, nlohmann::json>>& relationsFrom() const;

        /// @brief Gets all outgoing relations.
        /// @return Outgoing relations.
        const core::memory::TypeMap<std::unordered_map<std::string, nlohmann::json>>& relationsTo() const;

        /// @brief Inserts a relation of the given type into the node.
        /// @param type Relation type.
        /// @param entity Path to entity which the relation originates from.
        /// @param data Relation data.
        void relateFrom(const core::reflection::Type& type, const std::string& entity, nlohmann::json data);

        /// @brief Inserts a relation of the given type into the node.
        /// @param type Relation type.
        /// @param entity Path to entity which the relation points to.
        /// @param data Relation data.
        void relateTo(const core::reflection::Type& type, const std::string& entity, nlohmann::json data);

        /// @brief Removes the relation of the given type from the node.
        /// @param type Relation type.
        /// @param entity Path to entity which the relation originates from.
        void unrelateFrom(const core::reflection::Type& type, const std::string& entity);

        /// @brief Removes the relation of the given type from the node.
        /// @param type Relation type.
        /// @param entity Path to entity which the relation points to.
        void unrelateTo(const core::reflection::Type& type, const std::string& entity);

        /// @brief Gets the map of child nodes.
        /// @return Child nodes.
        const std::unordered_map<std::string, std::unique_ptr<SceneNode>>& children() const;

        /// @brief Creates a new child node with the given name.
        ///
        /// If a child node with the given name already exists, it is returned.
        ///
        /// @param name Name of the child node.
        /// @return Child node.
        SceneNode& create(const std::string& name);

        /// @brief Destroys the child node with the given name.
        /// @param name Name of the child node.
        void destroy(const std::string& name);

        /// @brief Loads the scene node from a JSON object.
        /// @param json JSON object.
        /// @param components Component type registry.
        /// @param relations Relation type registry.
        /// @return Whether the scene node was successfully loaded.
        bool load(const nlohmann::json& json, const core::reflection::TypeRegistry& components,
                  const core::reflection::TypeRegistry& relations);

    private:
        Asset<Scene> mInherits{};
        core::memory::TypeMap<nlohmann::json> mComponents;
        core::memory::TypeMap<std::unordered_map<std::string, nlohmann::json>> mRelationsFrom;
        core::memory::TypeMap<std::unordered_map<std::string, nlohmann::json>> mRelationsTo;
        std::unordered_map<std::string, std::unique_ptr<SceneNode>> mChildren;
    };
} // namespace cubos::engine
