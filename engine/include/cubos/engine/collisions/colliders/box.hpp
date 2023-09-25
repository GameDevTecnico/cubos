/// @file
/// @brief Component @ref cubos::engine::BoxCollider.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a box collider to an entity.
    ///
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies
    ///   inside the shape.
    /// - Positive margin: Requires a positive margin to round off sharp corners. Defined in
    ///   physics-space units.
    ///
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/box_collider", VecStorage)]] BoxCollider
    {
        glm::mat4 transform{1.0F};    ///< Transform of the collider.
        cubos::core::geom::Box shape; ///< Box shape of the collider.

        /// @brief Margin of the collider. Needed for collision stability.
        ///
        /// The collider margin avoids collision errors by rounding off sharp corners. It is
        /// absolute so the collider's transform won't affect it. Shouldn't be changed without good
        /// reason, as it's preferable to scale down the collider to account for it.
        float margin = 0.04F;
    };
} // namespace cubos::engine
