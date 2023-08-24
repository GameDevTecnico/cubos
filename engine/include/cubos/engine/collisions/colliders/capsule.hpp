/// @file
/// @brief Component @ref cubos::engine::CapsuleCollider.
/// @ingroup collisions-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/geom/capsule.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a capsule collider to an entity.
    ///
    /// - Convex: For any 2 points inside the shape, the line segment connecting those points lies
    /// inside the shape.
    /// - Zero margin: No margin is needed, since the shape has no sharp corners.
    ///
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/capsule_collider", VecStorage)]] CapsuleCollider
    {
        glm::mat4 transform{1.0f};        ///< Transform of the collider.
        cubos::core::geom::Capsule shape; ///< Capsule shape of the collider.
    };
} // namespace cubos::engine
