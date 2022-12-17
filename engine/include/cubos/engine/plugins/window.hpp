#ifndef CUBOS_ENGINE_PLUGINS_WINDOW_HPP
#define CUBOS_ENGINE_PLUGINS_WINDOW_HPP

#include <cubos/core/io/window.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins
{
    /// Plugin to create and handle the lifecycle of a window. This is required
    /// for showing any visual output to the user.
    ///
    /// Startup Stages:
    /// - openWindow: Opens the window.
    ///
    /// Stages:
    /// - poll: Polls the window for events and handles quit requests.
    /// - swapBuffers: Swaps the window's back buffers.
    /// @param cubos CUBOS. main class
    void windowPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_WINDOW_HPP
