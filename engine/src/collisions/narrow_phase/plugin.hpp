/// @dir
/// @brief @ref narrow-phase-collisions-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup narrow-phase-collisions-plugin

#pragma once

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup narrow-phase-collisions-plugin Narrow-phase Collisions
    /// @ingroup engine
    /// @brief Adds narrow-phase collision detection to @b Cubos
    ///
    /// ## Relations
    /// - @ref CollidingWith - relates two entities that are colliding.

    extern Tag collisionsNarrowCleanTag;

    extern Tag collisionsNarrowTag;

    extern Tag collisionsManifoldTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup narrow-phase-collisions-plugin
    void narrowPhaseCollisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
