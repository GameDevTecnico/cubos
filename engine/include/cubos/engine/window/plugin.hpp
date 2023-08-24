/// @dir
/// @brief Window plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/core/io/window.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup window-plugin Window
    /// @ingroup plugins
    /// @brief Creates and handles the lifecycle of a window.
    ///
    /// Initially sets @ref ShouldQuit to `false`, and sets it to `true` only when the window is
    /// closed.
    ///
    /// ## Settings
    /// - `window.title` - the window's title (default: `CUBOS.`).
    /// - `window.width` - the window's width (default: `800`).
    /// - `window.height` - the window's height (default: `600`).
    ///
    /// ## Events
    /// - @ref core::io::WindowEvent - event polled from the window.
    ///
    /// ## Resources
    /// - @ref core::io::Window - handle to the window.
    ///
    /// ## Startup tags
    /// - `cubos.window.init` - window is opened, runs after `cubos.settings`.
    ///
    /// ## Tags
    /// - `cubos.window.poll` - the window is polled for events, sending @ref core::io::WindowEvent's.
    /// - `cubos.window.render` - the window's back buffers are swapped.

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class
    /// @ingroup window-plugin
    void windowPlugin(Cubos& cubos);
} // namespace cubos::engine
