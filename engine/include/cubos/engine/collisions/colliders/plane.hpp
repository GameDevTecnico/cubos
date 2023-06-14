/// @file
/// @brief Contains the class for the Plane Collider component.

#pragma once

#include <cubos/core/geom/plane.hpp>

namespace cubos::engine
{
    /// @brief Plane Collider.
    struct [[cubos::component("cubos/plane_collider", VecStorage)]] PlaneCollider
    {
        glm::vec3 offset;               ///< The offset of the collider.
        cubos::core::geom::Plane shape; ///< The plane shape of the collider.
    };
}
