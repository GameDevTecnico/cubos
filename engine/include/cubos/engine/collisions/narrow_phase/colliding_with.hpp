/// @file
/// @brief Component @ref cubos::engine::CollidingWith.
/// @ingroup collisions-plugin

// FIXME: This should be private, but it's used in the sample.

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which represents a collision.
    /// @ingroup collisions-plugin
    struct CollidingWith
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to
        cubos::core::ecs::Entity other;  ///< The other entity in the collision
        float penetration;               ///< Penetration depth of the collision.
        glm::vec3 position;              ///< Position of contact on the surface of the entity.
        glm::vec3 normal;                ///< Normal of contact on the surface of the entity.
    };
} // namespace cubos::engine
