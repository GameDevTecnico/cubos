/// @file
/// @brief Class @ref cubos::engine::Scene.

#pragma once

#include <unordered_map>

#include <cubos/core/ecs/blueprint.hpp>

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    /// @brief The asset equivalent to ECS blueprints - a bundle of entities and their components.
    ///
    /// Scene assets produce a blueprint when loaded which can be used to spawn them. Entity names
    /// in the resulting blueprint follow the format "foo", "import1.bar", "import1.import2.baz",
    /// etc.
    ///
    /// @ingroup scene-plugin
    struct Scene
    {
        /// @brief Resulting blueprint which contains all the entities of the scene and its
        /// imported scenes. If you want to spawn the scene, use this blueprint.
        core::ecs::Blueprint blueprint;

        /// @brief Handles to other scenes that are imported by this scene, mapped by their import
        /// names. Imports of imports ar not included in this map.
        std::unordered_map<std::string, Asset<Scene>> imports;
    };
} // namespace cubos::engine
