#pragma once

#include <unordered_map>

#include <cubos/core/ecs/blueprint.hpp>

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    /// Stores the data of a scene. Entity names in the blueprint follow the format "importName.entityName".
    /// If the entity is in the root of the scene, then there is no importName nor the dot, so the entity name is just
    /// "entityName".
    struct Scene
    {
        /// The blueprint which contains all the entities of the scene and its imported scenes.
        /// If you want to spawn the scene, use this blueprint.
        core::ecs::Blueprint blueprint;

        /// Handles to other scenes that are imported by this scene, mapped by their names in the scene.
        /// Scenes imported by scenes imported by this scene are not included.
        std::unordered_map<std::string, Asset<Scene>> imports;
    };
} // namespace cubos::engine
