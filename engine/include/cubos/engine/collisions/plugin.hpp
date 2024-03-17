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
    /// @brief Adds collision detection to @b CUBOS.
    ///
    /// ## Components
    /// - @ref BoxCollisionShape - holds the box collision shape.
    /// - @ref CapsuleCollisionShape - holds the capsule collision shape.
    /// - @ref Collider - holds collider data.
    ///
    /// ## Events
    /// - @ref CollisionEvent - (TODO) emitted when a collision occurs.
    /// - @ref TriggerEvent - (TODO) emitted when a trigger is entered or exited.
    ///
    ///
    /// ## Dependencies
    /// - @ref transform-plugin

    /// @brief new colliders are setup.
    extern Tag CollisionsSetupTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup collisions-plugin
    void collisionsPlugin(Cubos& cubos);
} // namespace cubos::engine
