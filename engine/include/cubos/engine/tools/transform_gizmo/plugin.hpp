/// @dir
/// @brief @ref transform-gizmo-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup transform-gizmo-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup transform-gizmo-tool-plugin Transform Gizmo
    /// @ingroup tool-plugins
    /// @brief Adds a gizmo that allows changing the selected entity's position.
    ///
    /// ## Settings
    /// - `"transformGizmo.useLocalAxis"` - whether to use local instead of global axis (default: `true`).
    /// - `"transformGizmo.distanceToCamera"` - distance at which the gizmo is drawn, the larger the value the smaller
    /// the gizmo will be (default: `10`).

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup transform-gizmo-tool-plugin
    void transformGizmoPlugin(Cubos& cubos);
} // namespace cubos::engine
