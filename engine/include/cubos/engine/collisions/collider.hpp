/// @file
/// @brief Component @ref cubos::engine::Collider.
/// @ingroup collisions-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/geom/aabb.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a collider to an entity.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API Collider
    {
        CUBOS_REFLECT;

        glm::mat4 transform{1.0F}; ///< Transform of the collider.

        core::geom::AABB localAABB{}; ///< Local space AABB of the collider.
        core::geom::AABB worldAABB{}; ///< World space AABB of the collider.

        /// @brief Layer of the collider, from 0 to 63.
        uint64_t layer{0};

        /// @brief Mask of layers which the collider can collide with.
        ///
        /// Set to (1 << 0) to collide only with entities of layer 0, to (1 << 0) | (1 << 1) to collide with
        /// entities of layer 0 and 1, etc.
        ///
        /// By default, colliders collide with layer 0.
        uint64_t mask{1 << 0};

        /// @brief Margin of the collider.
        ///
        /// When the collider shape has sharp edges, a margin is needed.
        /// The plugin will set it based on the shape associated with the collider.
        float margin;
    };
} // namespace cubos::engine
