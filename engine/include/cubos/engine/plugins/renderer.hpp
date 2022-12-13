#ifndef CUBOS_ENGINE_PLUGINS_RENDERER_HPP
#define CUBOS_ENGINE_PLUGINS_RENDERER_HPP

#include <cubos/engine/ecs/transform_system.hpp>

#include <cubos/engine/cubos.hpp>

#include <glm/glm.hpp>
#include <cubos/core/gl/render_device.hpp>

namespace cubos::engine::plugins
{
    /// Plugin to create and use a renderer.
    ///
    /// Startup Stages:
    /// - setRenderer: Sets up the renderer.
    ///
    /// Stages:
    /// - draw: Draws component's to the renderer's frame.
    /// @param cubos CUBOS. main class
    void rendererPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_RENDERER_HPP