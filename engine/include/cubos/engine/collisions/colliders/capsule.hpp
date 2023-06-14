/// @file
/// @brief Contains the class for the Capsule Collier component.

#pragma once

#include <cubos/core/geom/capsule.hpp>

namespace cubos::engine
{
    /// @brief Capsule Collider.
    struct [[cubos::component("cubos/capsule_collider", VecStorage)]] CapsuleCollider
    {
        glm::mat4 transform;              ///< The transform of the collider.
        cubos::core::geom::Capsule shape; ///< The capsule shape of the collider.
    };
}
