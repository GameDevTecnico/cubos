/// @dir
/// @brief @ref renderer-plugin plugin directory.

/// @file
/// @brief Plugin entry point, resource @ref cubos::engine::ActiveCameras and components @ref
/// cubos::engine::RenderableGrid and @ref cubos::engine::Camera.
/// @ingroup renderer-plugin
/// @todo Move resources and components to their own files.

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/renderer/renderer.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>

namespace cubos::engine
{
    /// @defgroup renderer-plugin Renderer
    /// @ingroup engine
    /// @brief Creates and handles the lifecycle of a renderer.
    /// @see Take a look at the @ref examples-engine-renderer example for a demonstration of this
    /// plugin.
    ///
    /// Renders all entities with the @ref RenderableGrid component, using as cameras entities with
    /// the @ref Camera component selected by the @ref ActiveCameras resource. Lights are rendered
    /// using entities with @ref SpotLight, @ref DirectionalLight or @ref PointLight components.
    ///
    /// @note Entities with the above entities will be ignored if they do not possess
    /// @ref LocalToWorld components.
    ///
    /// The rendering environment, such as the ambient lighting and sky color, can be set through
    /// the resource @ref RendererEnvironment.
    ///
    /// ## Settings
    /// - `cubos.renderer.ssao.enabled` - whether SSAO is enabled.
    /// - `cubos.renderer.bloom.enabled` - whether bloom is enabled.
    ///
    /// ## Resources
    /// - @ref Renderer - handle to the renderer.
    /// - @ref RendererFrame - holds the current frame information.
    /// - @ref RendererEnvironment - holds the environment information (ambient light, sky gradient).
    /// - @ref ActiveCameras - holds the entities which represents the active cameras.
    /// - @ref ActiveVoxelPalette - holds an asset handle to the currently active palette.
    ///
    /// ## Components
    /// - @ref RenderableGrid - a grid to be rendered.
    /// - @ref Camera - holds camera information.
    /// - @ref Viewport - holds viewport information.
    /// - @ref SpotLight - emits a spot light.
    /// - @ref DirectionalLight - emits a directional light.
    /// - @ref PointLight - emits a point light.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref transform-plugin
    /// - @ref assets-plugin

    /// @brief the renderer is initialized, after `cubos.window.init`.
    extern Tag RendererInitTag;

    /// @brief frame information is collected, after `cubos.transform.update`.
    extern Tag RendererFrameTag;

    /// @brief frame is rendered to the window, after `cubos.renderer.frame` and
    /// before `cubos.window.render`.
    extern Tag RendererDrawTag;

    /// @brief Component which makes a voxel grid be rendered by the renderer plugin.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct RenderableGrid
    {
        CUBOS_REFLECT;

        Asset<VoxelGrid> asset;                ///< Handle to the grid asset to be rendered.
        glm::vec3 offset = {0.0F, 0.0F, 0.0F}; ///< Translation applied to the voxel grid before any other.
        RendererGrid handle = nullptr;         ///< Handle to the uploaded grid - set automatically.
    };

    /// @brief Resource which identifies the camera entities to be used by the renderer.
    /// @ingroup renderer-plugin
    struct ActiveCameras
    {
        /// @brief Entities which represent the current active cameras. If more than one is set,
        /// the screen is split. At most, 4 cameras are supported at the same time.
        /// @note These entities must have @ref Camera @ref LocalToWorld components.
        core::ecs::Entity entities[4];
    };

    /// @brief Resource which holds an asset handle to the currently active palette.
    /// @ingroup renderer-plugin
    struct ActiveVoxelPalette
    {
        /// @brief Asset handle to the currently active palette.
        Asset<VoxelPalette> asset;

        /// @brief Previous asset handle save in order to check if asset changed later.
        /// @todo ECS should have a .changed function to make this process easier (#273).
        Asset<VoxelPalette> prev;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup renderer-plugin
    void rendererPlugin(Cubos& cubos);
} // namespace cubos::engine
