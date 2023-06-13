/// @file
/// @brief Collision plugin entry point.

#pragma once

#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/colliders/capsule.hpp>
#include <cubos/engine/collisions/colliders/plane.hpp>
#include <cubos/engine/collisions/colliders/simplex.hpp>
#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// Plugin which adds collision handling to CUBOS.
    ///
    /// @details
    ///
    /// Components:
    /// - `BoxCollider`: holds the box collider data.
    /// - `CapsuleCollider`: holds the capsule collider data.
    /// - `PlaneCollider`: holds the plane collider data.
    /// - `SimplexCollider`: holds the simplex collider data.
    ///
    /// Events:
    /// - `CollisionEvent`: emitted when a collision occurs.
    /// - `TriggerEvent`: emitted when a trigger is entered or exited.
    ///
    /// Resources:
    /// - `BroadPhaseCollisions`: Stores broad phase collision data.
    ///
    /// Tags:
    /// - `cubos.collisions.broad.aabb`: collider aabbs have been updated.
    /// - `cubos.collisions.broad.pairs`: broad phase collision pairs have been found.
    /// - `cubos.collisions.broad`: broad phase has ended.
    /// - `cubos.collisions`: collisions have been resolved.
    ///
    /// Dependencies:
    /// - `transformPlugin`
    ///
    /// @param cubos CUBOS. main class.
    void collisionsPlugin(Cubos& cubos);
} // namespace cubos::engine