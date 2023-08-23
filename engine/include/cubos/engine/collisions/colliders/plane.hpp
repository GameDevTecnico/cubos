/// @file
/// @brief Component @ref cubos::engine::PlaneCollider.

#pragma once

#include <cubos/core/geom/plane.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a plane collider to an entity.
    ///
    /// - Zero thickness: Not suitable for dynamic objects, but can be used for static objects.
    /// - Positive margin: Requires a positive margin. Defined in physics-space units.
    ///
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/plane_collider", VecStorage)]] PlaneCollider
    {
        glm::vec3 offset{0.0f};         ///< Offset of the collider.
        cubos::core::geom::Plane shape; ///< Plane shape of the collider.

        /// @brief Margin of the collider. Needed for creating the AABB.
        ///
        /// The collider margin is absolute so the collider's transform won't affect it. Shouldn't
        /// be changed without good reason.
        float margin = 0.04f;
    };
} // namespace cubos::engine
