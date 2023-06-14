/// @file
/// Contains the class for the Box Collider component.

#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    /// @brief Box Collider.
    struct [[cubos::component("cubos/box_collider", VecStorage)]] BoxCollider
    {
        glm::mat4 transform{1.0f};    ///< The transform of the collider.
        cubos::core::geom::Box shape; ///< The box shape of the collider.
        float margin = 0.04f;         ///< The margin of the collider. Needed for collision stability.
    };
}
