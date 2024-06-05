/// @dir
/// @brief @ref tesseratos-collider-gizmos-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-collider-gizmos-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-collider-gizmos-plugin Collider gizmos
    /// @ingroup tesseratos
    /// @brief Draws gizmos for selected colliders.
    ///
    /// ## Dependencies
    /// - @ref gizmos-plugin
    /// - @ref collisions-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-collider-gizmos-plugin
    void colliderGizmosPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
