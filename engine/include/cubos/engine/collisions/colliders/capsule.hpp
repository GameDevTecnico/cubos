/// @file
/// @brief Contains the class for the Capsule Collier component.

#pragma once

#include <cubos/core/geom/capsule.hpp>

namespace cubos::engine
{
    /// @brief Capsule Collider.
    ///
    /// @details
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies inside the shape.
    /// - Zero margin: No margin is needed, since the shape has no sharp corners.
    struct [[cubos::component("cubos/capsule_collider", VecStorage)]] CapsuleCollider
    {
        glm::mat4 transform{1.0f};        ///< The transform of the collider.
        cubos::core::geom::Capsule shape; ///< The capsule shape of the collider.
    };
} // namespace cubos::engine
