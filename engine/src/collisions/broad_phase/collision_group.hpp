/// @file
/// @brief Component @ref cubos::engine::CollisionMask.
/// @ingroup collisions-plugin

#pragma once

#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component that contains the group of the collider and if it is static.
    /// @ingroup collisions-plugin
    struct CollisionGroup
    {
        CUBOS_REFLECT;

        /// @brief Group to which the collider belongs.
        ///
        /// The groupId is the index of the owner entity of the shape.
        ///
        /// Internally, it is used to identity whether the shapes should collide or if they constitute a bigger shape,
        /// as is the case when being a sub-shape of a MultiCollisionShape. In case they are unique, they will be
        /// attributed their entity index, otherwise, they will be attributed the index of the entity containing the
        /// MultiCollisionShape component (their parent).
        ///
        /// By default, this is set automatically.
        uint32_t groupId;

        /// @brief Identities if a collider is static.
        ///
        /// Internally, it works as a special groupId. It is used to check whether two bodies should collide. For
        /// optimization, two static bodies do not collide even when they are intersecting.
        ///
        /// By default, this is set automatically.
        bool isStatic;
    };
} // namespace cubos::engine
