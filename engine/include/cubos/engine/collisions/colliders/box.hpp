/// @file
/// Contains the class for the Box Collider component.

#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    /// @brief Box Collider.
    ///
    /// @details
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies inside the shape.
    /// - Positive margin: Requires a positive margin to round off sharp corners. Defined in physics-space units.
    struct [[cubos::component("cubos/box_collider", VecStorage)]] BoxCollider
    {
        glm::mat4 transform{1.0f};    ///< The transform of the collider.
        cubos::core::geom::Box shape; ///< The box shape of the collider.

        /// @brief The margin of the collider. Needed for collision stability.
        ///
        /// @details
        /// The collider margin avoids collision errors by rounding off sharp corners.
        /// It is absolute so the collider's transform won't affect it.
        /// Shouldn't be changed without good reason, it's preferable to scale down the collider to account for it.
        float margin = 0.04f;
    };
}
