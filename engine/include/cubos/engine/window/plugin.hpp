/// @dir
/// @brief @ref window-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup window-plugin

#pragma once

#include <cubos/core/io/window.hpp>

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup window-plugin Window
    /// @ingroup engine
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
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief window is opened, runs after `cubos.settings`.
    extern Tag windowInitTag;

    /// @brief the window is polled for events, sending @ref core::io::WindowEvent's.
    extern Tag windowPollTag;

    /// @brief the window's back buffers are swapped.
    extern Tag windowRenderTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup window-plugin
    void windowPlugin(Cubos& cubos);
} // namespace cubos::engine
