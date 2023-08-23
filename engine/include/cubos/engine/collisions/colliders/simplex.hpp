/// @file
/// @brief Component @ref cubos::engine::SimplexCollider.

#pragma once

#include <cubos/core/geom/simplex.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a simplex collider to an entity.
    ///
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies
    ///   inside the shape.
    /// - Positive margin: Requires a positive margin to round off sharp corners. Defined in
    ///   physics-space units.
    /// - Zero thickness (up to 3 points): Not suitable for dynamic objects, but can be used for
    ///   static objects.
    ///
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/simplex", VecStorage)]] SimplexCollider
    {
        glm::vec3 offset{0.0f};           ///< Offset of the collider.
        cubos::core::geom::Simplex shape; ///< Simplex shape of the collider.

        /// @brief Margin of the collider. Needed for collision stability.
        ///
        /// The collider margin avoids collision errors by rounding off sharp corners. Shouldn't be
        /// changed without good reason, it's preferable to move the simplex corners to account for
        /// it.
        float margin = 0.04f;
    };
} // namespace cubos::engine
