/// @dir
/// @brief @ref tesseratos-transform-gizmo-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-transform-gizmo-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-transform-gizmo-plugin Transform gizmo
    /// @ingroup tesseratos
    /// @brief Add a gizmo that allows changing an entity's position.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-transform-gizmo-plugin
    void transformGizmoPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
