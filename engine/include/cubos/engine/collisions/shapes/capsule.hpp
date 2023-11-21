/// @file
/// @brief Component @ref cubos::engine::CapsuleCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/capsule.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a capsule collision shape to an entity, used with a @ref Collider component.
    /// @ingroup collisions-plugin
    struct CapsuleCollisionShape
    {
        CUBOS_REFLECT;

        cubos::core::geom::Capsule capsule; ///< Capsule shape.
    };
} // namespace cubos::engine
