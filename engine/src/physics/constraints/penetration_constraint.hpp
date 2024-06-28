/// @file
/// @brief Component @ref cubos::engine::PenetrationConstraint.
/// @ingroup physics-solver-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Relation which holds the information for resolving a penetration between particles.
    /// @ingroup physics-solver-plugin
    struct PenetrationConstraint
    {
        CUBOS_REFLECT;

        // colision info
        cubos::core::ecs::Entity entity;   ///< Entity to which the normal is relative to.
        glm::vec3 entity1OriginalPosition; ///< Position of the entity when the collision occured. (could be replaced by
                                           ///< contact point in the future)
        glm::vec3 entity2OriginalPosition; ///< Position of the other entity when the collision occured. (could be
                                           ///< replaced by contact point in the future)
        float penetration;                 ///< Penetration depth of the collision.
        glm::vec3 normal;                  ///< Normal of contact on the surface of the entity.

        float normalMass;

        // soft constraint
        float biasCoefficient;
        float impulseCoefficient;
        float massCoefficient;
    };
} // namespace cubos::engine
