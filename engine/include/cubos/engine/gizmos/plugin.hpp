/// @dir
/// @brief @ref gizmos-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup gizmos-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/gizmos/gizmos.hpp>

namespace cubos::engine
{
    /// @defgroup gizmos-plugin Gizmos
    /// @ingroup engine
    /// @brief Allows gizmos to be drawn.
    ///
    /// ## Resources
    /// - @ref Gizmos - stores gizmos information.
    ///

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup gizmos-plugin
    void gizmosPlugin(Cubos& cubos);
} // namespace cubos::engine
