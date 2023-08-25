/// @dir
/// @brief @ref renderer-plugin plugin directory.

/// @file
/// @brief Plugin entry point, resource @ref cubos::engine::ActiveCameras and components @ref
/// cubos::engine::RenderableGrid and @ref cubos::engine::Camera.
/// @ingroup renderer-plugin
/// @todo Move resources and components to their own files.

#pragma once

#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/palette.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/renderer/renderer.hpp>

namespace cubos::engine
{
    /// @defgroup renderer-plugin Renderer
    /// @ingroup engine
    /// @brief Creates and handles the lifecycle of a renderer.
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
    ///
    /// ## Resources
    /// - @ref Renderer - handle to the renderer.
    /// - @ref RendererFrame - holds the current frame information.
    /// - @ref RendererEnvironment - holds the environment information (ambient light, sky gradient).
    /// - @ref ActiveCameras - holds the entities which represents the active cameras.
    ///
    /// ## Components
    /// - @ref RenderableGrid - a grid to be rendered.
    /// - @ref Camera - holds camera information.
    /// - @ref SpotLight - emits a spot light.
    /// - @ref DirectionalLight - emits a directional light.
    /// - @ref PointLight - emits a point light.
    ///
    /// ## Startup tags
    /// - `cubos.renderer.init` - the renderer is initialized, after `cubos.window.init`.
    ///
    /// ## Tags
    /// - `cubos.renderer.frame` - frame information is collected, after `cubos.transform.update`.
    /// - `cubos.renderer.draw` - frame is rendered to the window, after `cubos.renderer.frame` and
    ///   before `cubos.window.render`.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref transform-plugin
    /// - @ref assets-plugin

    /// @brief Component which makes a voxel grid be rendered by the renderer plugin.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/renderable_grid", VecStorage)]] RenderableGrid
    {
        Asset<core::gl::Grid> asset;                     ///< Handle to the grid asset to be rendered.
        glm::vec3 offset = {0.0F, 0.0F, 0.0F};           ///< Translation applied to the voxel grid before any other.
        [[cubos::ignore]] RendererGrid handle = nullptr; ///< Handle to the uploaded grid - set automatically.
    };

    /// @brief Component which defines parameters of a camera used to render the world.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/camera", VecStorage)]] Camera
    {
        float fovY;  ///< Vertical field of view in degrees.
        float zNear; ///< Near clipping plane.
        float zFar;  ///< Far clipping plane.
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

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup renderer-plugin
    void rendererPlugin(Cubos& cubos);
} // namespace cubos::engine
