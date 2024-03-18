/// @file
/// @brief Component @ref cubos::engine::Collider.
/// @ingroup collisions-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/geom/aabb.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a collider to an entity.
    /// @ingroup collisions-plugin
    struct Collider
    {
        CUBOS_REFLECT;

        glm::mat4 transform{1.0F}; ///< Transform of the collider.

        core::geom::AABB localAABB{}; ///< Local space AABB of the collider.
        core::geom::AABB worldAABB{}; ///< World space AABB of the collider.

        /// @brief Margin of the collider.
        ///
        /// When the collider shape has sharp edges, a margin is needed.
        /// The plugin will set it based on the shape associated with the collider.
        float margin;
    };
} // namespace cubos::engine
