/// @file
/// @brief Component @ref cubos::engine::Collider.
/// @ingroup collisions-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/engine/collisions/aabb.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a collider to an entity.
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/collider", VecStorage)]] Collider
    {
        glm::mat4 transform{1.0F}; ///< Transform of the collider.

        bool fresh = true;      ///< Whether the collider is fresh. This is an hack and should be done in ECS.
        ColliderAABB localAABB; ///< Local space AABB of the collider.
        ColliderAABB worldAABB; ///< World space AABB of the collider.
    };
} // namespace cubos::engine
