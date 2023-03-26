#pragma once

#include <cubos/core/io/window.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins
{
    /// Plugin to create and handle the lifecycle of a window. This is required for showing any
    /// visual output to the user.
    ///
    /// @details This plugin adds three systems: one to open the window, one to poll the window
    /// for events, and one to swap the window's back buffers. Polled events are written to the
    /// event pipe as `core::io::WindowEvent`s.
    ///
    /// Sets `ShouldQuit` to `true` if the window is closed. The window is created with the
    /// settings `window.width`, `window.height` and `window.title`.
    ///
    /// Events:
    /// - `core::io::WindowEvent`: events polled from the window.
    ///
    /// Resources:
    /// - `core::io::Window`: the window's handle.
    ///
    /// Startup tags:
    /// - `cubos.window.init`: initializes and opens the window, runs after `cubos.settings`.
    ///
    /// Tags:
    /// - `cubos.window.poll`: polls the window for events.
    /// - `cubos.window.render`: swaps the window's back buffers.
    ///
    /// @param cubos CUBOS. main class
    void windowPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins
