#ifndef CUBOS_ENGINE_PLUGINS_RENDERER_HPP
#define CUBOS_ENGINE_PLUGINS_RENDERER_HPP

#include <cubos/engine/ecs/transform_system.hpp>

#include <cubos/engine/cubos.hpp>

#include <glm/glm.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/ecs/entity_manager.hpp>

namespace cubos::engine::plugins
{
    /// Plugin to create and use a renderer.
    ///
    /// Startup Tags:
    /// - setRenderer: Sets up the renderer.
    ///
    /// Tags:
    /// - CreateDrawList: Creates list of entities to be drawn in a frame.
    /// - Draw: Draws component's to the renderer's frame.
    /// @param cubos CUBOS. main class
    void rendererPlugin(Cubos& cubos);

    /// Resource to contain the current active camera
    struct ActiveCamera
    {
        cubos::core::ecs::Entity entity;
    };
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_RENDERER_HPP