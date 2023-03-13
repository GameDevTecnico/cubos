#ifndef CUBOS_ENGINE_PLUGINS_RENDERER_HPP
#define CUBOS_ENGINE_PLUGINS_RENDERER_HPP

#include <cubos/engine/cubos.hpp>

#include <cubos/core/ecs/entity_manager.hpp>

namespace cubos::engine::plugins
{
    /// Resource to identify the entity which represents the active camera.
    struct ActiveCamera
    {
        core::ecs::Entity entity;
    };

    /// Plugin to create and handle the lifecycle of a renderer. It renders all entities with a
    /// `Grid` and `LocalToWorld` components.
    ///
    /// @details This plugin adds three systems: one to initialize the renderer, one to collect the
    /// frame information and one to draw the frame.
    ///
    /// Dependencies:
    /// - `windowPlugin`
    /// - `transformPlugin`
    ///
    /// Resources:
    /// - `Renderer`: handle to the renderer.
    /// - `Frame`: holds the current frame information.
    /// - `ActiveCamera`: the entity which represents the active camera.
    ///
    /// Components:
    /// - `Grid`: a grid to be rendered. Must be used with `LocalToWorld`.
    /// - `Camera`: holds camera information. The entity pointed to by `ActiveCamera` must have
    ///   this component.
    ///
    /// Startup tags:
    /// - `cubos.renderer.init`: initializes the renderer, after `cubos.window.init`.
    ///
    /// Tags:
    /// - `cubos.renderer.frame`: collects the frame information, after `cubos.transform.update`.
    /// - `cubos.renderer.draw`: renders the frame to the window, after `cubos.renderer.frame`
    ///   and before `cubos.window.render`.
    ///
    /// @param cubos CUBOS. main class
    void rendererPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_RENDERER_HPP
