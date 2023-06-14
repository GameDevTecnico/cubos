/// @file
/// @brief Contains the class for the Simplex Collider component.

#pragma once

#include <cubos/core/geom/simplex.hpp>

namespace cubos::engine
{
    /// @brief Simplex Collider.
    struct [[cubos::component("cubos/simplex", VecStorage)]] SimplexCollider
    {
        glm::vec3 offset;                 ///< The offset of the collider.
        cubos::core::geom::Simplex shape; ///< The simplex shape of the collider.
        float margin;                     ///< The margin of the collider. Needed for collision stability.
    };
}
