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
    struct [[cubos::component("cubos/plane_collider", VecStorage)]] PlaneCollider
    {
        glm::vec3 offset{0.0f};         ///< The offset of the collider.
        cubos::core::geom::Plane shape; ///< The plane shape of the collider.
    };
}
