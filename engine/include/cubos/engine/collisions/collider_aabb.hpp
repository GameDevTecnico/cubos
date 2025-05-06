/// @file
/// @brief Component @ref cubos::engine::ColliderAABB.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/aabb.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which adds an collider AABB to an entity.
    ///
    /// Added automatically when any collision shape is added to the entity.
    ///
    /// @ingroup collisions-plugin
    struct ColliderAABB
    {
        CUBOS_REFLECT;

        core::geom::AABB localAABB{}; ///< Local space AABB of the collider.
        core::geom::AABB worldAABB{}; ///< World space AABB of the collider.

        /// @brief Margin of the collider.
        ///
        /// When the collider shape has sharp edges, a margin is needed.
        /// The plugin will set it based on the shape associated with the collider.
        float margin;
    };
} // namespace cubos::engine
