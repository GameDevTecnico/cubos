/// @dir
/// @brief @ref collisions-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup collisions-plugin

/// @dir ./shapes
/// @brief Collision shapes components directory.

/// @dir ./broad_phase
/// @brief Broad phase collision sub-plugin directory.

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>

namespace cubos::engine
{

    /// @defgroup collisions-plugin Collisions
    /// @ingroup engine
    /// @brief Adds collision detection to @b Cubos
    ///
    /// ## Components
    /// - @ref BoxCollisionShape - holds a box collision shape for a Collider.
    /// - @ref CapsuleCollisionShape - holds a capsule collision shape for a Collider.
    /// - @ref VoxelCollisionShape - holds a voxel collision shape for a Collider.
    /// - @ref ColliderAABB - holds collider AABB data.
    /// - @ref CollisionLayers - holds the collision layers where the collider appears in.
    /// - @ref CollisionMask - holds the mask of layers which the collider can collide with.
    /// - @ref Collider - identifies an entity as the owner of a Collider and its settings.
    ///
    /// ## Bundles
    /// - @ref ColliderBundle - contains all the necessary components for an entity to have collision detection (A
    /// CollisionShape must be added separately).
    ///
    /// ## Relations
    /// - @ref IntersectionStart - Relates entities during the frame when two entities start colliding.
    /// - @ref CollidingWith - Relates entities while they are colliding.
    /// - @ref IntersectionEnd - Relates entities during the frame when two entities stop colliding.
    ///
    /// ## Dependencies
    /// - @ref transform-plugin

    /// @brief Detects collisions and updates colliders.
    CUBOS_ENGINE_API extern Tag collisionsTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup collisions-plugin
    CUBOS_ENGINE_API void collisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
