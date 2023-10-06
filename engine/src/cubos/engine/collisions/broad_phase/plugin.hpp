/// @dir
/// @brief @ref broad-phase-collisions-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup broad-phase-collisions-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup broad-phase-collisions-plugin Broad-phase Collisions
    /// @ingroup engine
    /// @brief Adds broad-phase collision detection to @b CUBOS.
    ///
    /// ## Resources
    /// - @ref BroadPhaseCandidates - stores broad phase collision data.
    /// - @ref BroadPhaseSweepAndPrune - stores sweep and prune markers.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup broad-phase-collisions-plugin
    void broadPhaseCollisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
