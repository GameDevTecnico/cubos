/// @file
/// @brief Event @ref cubos::engine::CollisionEvent.
/// @ingroup collisions-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>

namespace cubos::engine
{
    /// @brief Represents a collision event.
    /// @ingroup collisions-plugin
    struct CollisionEvent
    {
        cubos::core::ecs::Entity entity; ///< Entity involved in the collision.
        cubos::core::ecs::Entity other;  ///< Other entity involved in the collision.
        float penetration;               ///< Penetration depth of the collision.
        glm::vec3 position;              ///< Position of contact on the surface of the entity.
        glm::vec3 normal;                ///< Normal of contact on the surface of the entity.
    };
}; // namespace cubos::engine
