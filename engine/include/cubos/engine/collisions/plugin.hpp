/// @dir
/// @brief Collisions plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup collisions-plugin

/// @dir ./colliders
/// @brief Collider components directory.

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup collisions-plugin Collisions
    /// @ingroup plugins
    /// @brief Adds collision detection to CUBOS.
    ///
    /// ## Components
    /// - @ref BoxCollider - holds the box collider data.
    /// - @ref CapsuleCollider - holds the capsule collider data.
    /// - @ref PlaneCollider - holds the plane collider data.
    /// - @ref SimplexCollider - holds the simplex collider data.
    ///
    /// ## Events
    /// - @ref CollisionEvent - (TODO) emitted when a collision occurs.
    /// - @ref TriggerEvent - (TODO) emitted when a trigger is entered or exited.
    ///
    /// ## Resources
    /// - @ref BroadPhaseCollisions - stores broad phase collision data.
    ///
    /// ## Tags
    /// - `cubos.collisions.aabb.missing` - missing aabb colliders are added.
    /// - `cubos.collisions.aabb` - collider aabbs are updated.
    /// - `cubos.collisions.broad.markers` - sweep markers are updated.
    /// - `cubos.collisions.broad.sweep` - sweep is performed.
    /// - `cubos.collisions.broad` - broad phase collision detection.
    /// - `cubos.collisions` - collisions are resolved.
    ///
    /// ## Dependencies
    /// - @ref transform-plugin

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class.
    /// @ingroup collisions-plugin
    void collisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
