/// @file
/// @brief Contains the class for the Simplex Collider component.

#pragma once

#include <cubos/core/geom/simplex.hpp>

namespace cubos::engine
{
    /// @brief Simplex Collider.
    ///
    /// @details
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies inside the shape.
    /// - Positive margin: Requires a positive margin to round off sharp corners. Defined in physics-space units.
    /// - Zero thickness (up to 3 points): Not suitable for dynamic objects, but can be used for static objects.
    struct [[cubos::component("cubos/simplex", VecStorage)]] SimplexCollider
    {
        glm::vec3 offset{0.0f};           ///< The offset of the collider.
        cubos::core::geom::Simplex shape; ///< The simplex shape of the collider.

        /// @brief The margin of the collider. Needed for collision stability.
        ///
        /// @details
        /// The collider margin avoids collision errors by rounding off sharp corners.
        /// Shouldn't be changed without good reason, it's preferable to move the simplex corners to account for it.
        float margin = 0.04f;
    };
}
