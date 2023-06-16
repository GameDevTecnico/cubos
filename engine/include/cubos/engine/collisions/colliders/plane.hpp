/// @file
/// @brief Contains the class for the Plane Collider component.

#pragma once

#include <cubos/core/geom/plane.hpp>

namespace cubos::engine
{
    /// @brief Plane Collider.
    ///
    /// @details
    /// - Zero thickness: Not suitable for dynamic objects, but can be used for static objects.
    /// - Positive margin: Requires a positive margin. Defined in physics-space units.
    struct [[cubos::component("cubos/plane_collider", VecStorage)]] PlaneCollider
    {
        glm::vec3 offset{0.0f};         ///< The offset of the collider.
        cubos::core::geom::Plane shape; ///< The plane shape of the collider.

        /// @brief The margin of the collider. Needed for creating the AABB.
        ///
        /// @details.
        /// The collision margin is absolute so the collider's transform won't affect it.
        /// Shouldn't be changed without good reason.
        float margin = 0.04f;
    };
} // namespace cubos::engine
