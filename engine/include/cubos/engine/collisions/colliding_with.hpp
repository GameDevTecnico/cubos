/// @file
/// @brief Relation @ref cubos::engine::CollidingWith.
/// @ingroup collisions-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Relation which represents a collision.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API CollidingWith
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to.
        glm::vec3 entity1OriginalPosition;
        glm::vec3 entity2OriginalPosition;
        float penetration;  ///< Penetration depth of the collision.
        glm::vec3 position; ///< Position of contact on the surface of the entity.
        glm::vec3 normal;   ///< Normal of contact on the surface of the entity.
    };
} // namespace cubos::engine
