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
    /// ## Dependencies
    /// - @ref screen-picker-plugin
    /// - @ref window-plugin

    /// @brief the gizmos renderer is initialized, after `cubos.window.init`
    extern Tag gizmosInitTag;

    /// @brief gizmos interaction is handled, after `cubos.window.poll` and
    /// before `cubos.gizmos.draw`.
    extern Tag gizmosInputTag;

    /// @brief queued gizmos are rendered to the window, after `cubos.renderer.draw` and
    /// before `cubos.window.render`.
    extern Tag gizmosDrawTag;

    /// @brief the ScreenPicker resource is accessed to detect gizmos at mouse coordinates, after
    /// `cubos.gizmos.draw`.
    extern Tag gizmosPickTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup gizmos-plugin
    void gizmosPlugin(Cubos& cubos);
} // namespace cubos::engine
