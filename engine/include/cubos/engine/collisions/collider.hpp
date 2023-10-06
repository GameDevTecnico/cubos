/// @file
/// @brief Component @ref cubos::engine::Collider.
/// @ingroup collisions-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/geom/aabb.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a collider to an entity.
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/collider", VecStorage)]] Collider
    {
        glm::mat4 transform{1.0F}; ///< Transform of the collider.

        core::geom::AABB localAABB{}; ///< Local space AABB of the collider.
        core::geom::AABB worldAABB{}; ///< World space AABB of the collider.

        /// @brief Margin of the collider.
        ///
        /// When the collider shape has sharp edges, a margin is needed.
        /// The plugin will set it based on the shape associated with the collider.
        float margin;

        int fresh = -1; ///< This is an hack and should be done in ECS.
    };
} // namespace cubos::engine
