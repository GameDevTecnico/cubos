/// @file
/// @brief Component @ref cubos::engine::RenderVoxelGrid.
/// @ingroup render-voxels-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace cubos::engine
{
    /// @brief Component used to draw voxel grids.
    ///
    /// When added to an entity, the grid may not be able to be rendered immediately, as the asset must load first, and
    /// the necessary data must be sent to the GPU. A @ref LoadRenderVoxels component is automatically added to the
    /// entity by an observer, which can be used to track the grid's loading progress. When the grid is loaded, the task
    /// is removed.
    ///
    /// If the grid is changed, in order to update the rendering, a new @ref LoadRenderVoxels component must be added to
    /// the entity, manually.
    ///
    /// @ingroup render-voxels-plugin
    struct CUBOS_ENGINE_API RenderVoxelGrid
    {
        CUBOS_REFLECT;

        /// @brief Handle to the grid asset to be rendered.
        ///
        /// When null, no grid will be rendered.
        Asset<VoxelGrid> asset{};

        /// @brief Translation applied to the grid.
        glm::vec3 offset = {0.0F, 0.0F, 0.0F};
    };
} // namespace cubos::engine
