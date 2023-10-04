/// @file
/// @brief Component @ref cubos::engine::BoxCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a box collision shape to an entity, used with a collider component.
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/box_collision_shape", VecStorage)]] BoxCollisionShape
    {
        cubos::core::geom::Box shape; ///< Box shape.

        /// @brief Margin of the collider. Needed for collision stability.
        ///
        /// The collider margin avoids collision errors by rounding off sharp corners. It is
        /// absolute so the collider's transform won't affect it. Shouldn't be changed without good
        /// reason, as it's preferable to scale down the collider shape to account for it.
        float margin = 0.04F;
    };
} // namespace cubos::engine
