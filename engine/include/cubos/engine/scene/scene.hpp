/// @file
/// @brief Class @ref cubos::engine::Scene.
/// @ingroup scene-plugin

#pragma once

#include <unordered_map>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/scene/node.hpp>

namespace cubos::engine
{
    /// @brief Asset which holds a blueprint generated from a scene file.
    ///
    /// A scene represents an entity, its components and relations, and its children entities (i.e, sub-scenes).
    /// Optionally, a scene can inherit another scene asset, which means that it inherits all the entities and
    /// components.
    ///
    /// Scene assets produce a blueprint when loaded which can be used to spawn them. Spawned entities are given names
    /// based on the names of the entities in the scene file. The root entity is given the name of the scene file.
    /// Child entities are given the name of their parent entity followed by a '#' and the name of the child entity in
    /// the file.
    ///
    /// Scene assets are loaded from JSON files with the .cubos extension. They follow the format:
    ///
    /// @code json
    /// {
    ///     "inherit": "ba19170e-65a0-47cd-9bd2-b4ea9447fe22",
    ///     "game::Player": {},
    ///     "#camera": {
    ///         "cubos::engine::PerspectiveCamera": {
    ///             "fov": 90,
    ///         }
    ///     },
    ///     "#gun": {
    ///         "inherit": "6cb09eea-4156-4a75-b0ae-488aac843e05"
    ///     }
    /// }
    /// @endcode
    ///
    /// In this example, the scene first inherits the scene with the UUID "ba19170e-65a0-47cd-9bd2-b4ea9447fe22".
    /// Then, it adds a "game::Player" component to the root entity.
    /// It also adds (or modifies) a child entity with the name "camera", adding a component
    /// "cubos::engine::PerspectiveCamera". Finally, it adds a child entity with the name "gun" which inherits
    /// the scene with the UUID "6cb09eea-4156-4a75-b0ae-488aac843e05".
    ///
    /// @ingroup scene-plugin
    class CUBOS_ENGINE_API Scene
    {
    public:
        CUBOS_REFLECT;

        /// @brief Constructs an empty scene.
        Scene() = default;

        /// @brief Move constructs a scene.
        Scene(Scene&&) = default;

        /// @brief Forbid copy construction.
        Scene(const Scene&) = delete;

        /// @brief Sets the root node of the scene.
        /// @param root Root node.
        /// @param assets Assets manager used to load sub-scenes.
        /// @return Whether all data was successfully deserialized.
        bool loadFromNode(SceneNode root, const Assets& assets);

        /// @brief Gets the root node of the scene.
        /// @return Root node.
        const SceneNode& node() const;

        /// @brief Gets the blueprint generated from the scene node tree.
        /// @return Blueprint.
        const core::ecs::Blueprint& blueprint() const;

        /// @brief Overload which casts the scene reference to a reference to its blueprint.
        /// @param scene Scene.
        /// @return Blueprint.
        operator const core::ecs::Blueprint&() const
        {
            return blueprint();
        }

    private:
        /// @brief Creates the entities present in the given node sub-tree in the blueprint.
        /// @param node Root node of the sub-tree.
        /// @param assets Assets manager used to load sub-scenes.
        /// @param name Name of the entity of the root node.
        /// @return Entity of the root node.
        core::ecs::Entity loadEntities(const SceneNode& node, const Assets& assets, std::string name);

        /// @brief Adds components and relations of the given node to the blueprint.
        /// @param node Node to get the components and relations from.
        /// @param name Name of the entity of the root node.
        /// @return Whether all data was successfully deserialized.
        bool loadComponentsAndRelations(const SceneNode& node, const std::string& name);

        /// @brief Node representing the root entity of the scene.
        SceneNode mRoot;

        /// @brief Map of entities in the node tree to entities in the blueprint.
        std::unordered_map<std::string, core::ecs::Entity> mEntityMap;

        /// @brief Resulting blueprint which contains all of the entities loaded from the scene file and its
        /// dependencies.
        core::ecs::Blueprint mBlueprint;
    };
} // namespace cubos::engine
