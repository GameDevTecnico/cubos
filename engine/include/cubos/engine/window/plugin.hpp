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
    /// - `window.vSync` - whether vertical synchronization is enabled (default: `true`).
    ///
    /// ## Events
    /// - @ref core::io::WindowEvent - event polled from the window.
    ///
    /// ## Resources
    /// - @ref core::io::Window - handle to the window.
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Window is opened, runs after @ref settingsTag.
    CUBOS_ENGINE_API extern Tag windowInitTag;

    /// @brief The window is polled for events, sending @ref core::io::WindowEvent's.
    CUBOS_ENGINE_API extern Tag windowPollTag;

    /// @brief The window's back buffers are swapped.
    CUBOS_ENGINE_API extern Tag windowRenderTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup window-plugin
    CUBOS_ENGINE_API void windowPlugin(Cubos& cubos);
} // namespace cubos::engine

// We need to declare a specific instantiation of the event pipe for the event to make sure there's only one
// instance of the type when compiling with shared libraries.
CUBOS_ENGINE_EXTERN template class CUBOS_ENGINE_API cubos::core::ecs::EventPipe<cubos::core::io::WindowEvent>;
