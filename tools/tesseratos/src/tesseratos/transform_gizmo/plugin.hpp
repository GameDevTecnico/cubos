/// @dir
/// @brief @ref tesseratos-transform-gizmo-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-transform-gizmo-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-transform-gizmo-plugin Transform gizmo
    /// @ingroup tesseratos
    /// @brief Add a gizmo that allows changing an entity's position.
    ///
    /// ## Settings
    /// - `"transformGizmo.useLocalAxis"` - whether to use local instead of global axis (default: `true`).
    /// - `"transformGizmo.distanceToCamera"` - distance at which the gizmo is drawn, the larger the value the smaller
    /// the gizmo will be (default: `10`).

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-transform-gizmo-plugin
    void transformGizmoPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
