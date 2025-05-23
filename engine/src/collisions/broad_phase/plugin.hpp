/// @dir
/// @brief @ref broad-phase-collisions-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup broad-phase-collisions-plugin

#pragma once

#include <cubos/engine/collisions/collider_aabb.hpp>
#include <cubos/engine/prelude.hpp>

#include "potentially_colliding_with.hpp"

namespace cubos::engine
{
    /// @defgroup broad-phase-collisions-plugin Broad-phase Collisions
    /// @ingroup engine
    /// @brief Adds broad-phase collision detection to @b Cubos
    ///
    /// ## Relations
    /// - @ref PotentiallyCollidingWith - relates two entities that may be colliding
    ///
    /// ## Resources
    /// - @ref BroadPhaseSweepAndPrune - stores sweep and prune markers.

    extern Tag collisionsAABBSetupTag;

    extern Tag collisionsAABBUpdateTag;

    extern Tag collisionsBroadMarkersTag;

    extern Tag collisionsBroadSweepTag;

    extern Tag collisionsBroadCleanTag;

    extern Tag collisionsBroadTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup broad-phase-collisions-plugin
    void broadPhaseCollisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
