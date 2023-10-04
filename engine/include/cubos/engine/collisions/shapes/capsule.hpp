/// @file
/// @brief Component @ref cubos::engine::CapsuleCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/capsule.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a capsule collision shape to an entity, used with a @ref Collider component.
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/capsule_collision_shape", VecStorage)]] CapsuleCollisionShape
    {
        cubos::core::geom::Capsule capsule; ///< Capsule shape.
    };
} // namespace cubos::engine
