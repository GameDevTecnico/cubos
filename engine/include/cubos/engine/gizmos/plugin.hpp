/// @dir
/// @brief @ref gizmos-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup gizmos-plugin

#pragma once

#include <cubos/engine/gizmos/gizmos.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup gizmos-plugin Gizmos
    /// @ingroup engine
    /// @brief Used to draw gizmos helpful for debugging and tools.
    ///
    /// ## Resources
    /// - @ref Gizmos - used to queue gizmo draw commands.
    ///
    /// ## Startup tags
    /// - `cubos.gizmos.init` - the gizmos renderer is initialized, after `cubos.window.init`
    ///
    /// ## Tags
    /// - `cubos.gizmos.input` - gizmos interaction is handled, after `cubos.window.poll` and
    ///   before `cubos.gizmos.draw`.
    /// - `cubos.gizmos.draw` - queued gizmos are rendered to the window, after `cubos.renderer.draw` and
    ///   before `cubos.window.render`.
    /// - `cubos.gizmos.pick` - the ScreenPicker resource is accessed to detect gizmos at mouse coordinates, after
    /// `cubos.gizmos.draw`
    ///
    /// ## Dependencies
    /// - @ref screen-picker-plugin
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup gizmos-plugin
    void gizmosPlugin(Cubos& cubos);
} // namespace cubos::engine
