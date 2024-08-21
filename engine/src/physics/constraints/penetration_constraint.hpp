/// @file
/// @brief Component @ref cubos::engine::PenetrationConstraint.
/// @ingroup physics-solver-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    struct PenetrationConstraintPointData
    {
        /// TODO: should contain what contact point has but with also information to resolve the constraint
        /// TODO: should accumulated things be here?
        float initialPenetration;
        glm::vec3 initialRelativeVelocity; // I believe this depends on rotation speed at the point so should be here
    };

    /// @brief Relation which holds the information for resolving a penetration between particles.
    /// @ingroup physics-solver-plugin
    struct PenetrationConstraint
    {
        CUBOS_REFLECT;

        // colision info
        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to.
        float penetration;               ///< Penetration depth of the collision. This is to be removed
        glm::vec3 normal;                ///< Normal of contact on the surface of the entity. (world space)

        // separation
        float normalMass; ///< Mass to use for normal impulse calculation. (can these be here or should be by point (in
                          ///< which case it always depends probably))
        float normalImpulse; ///< Accumulated impulse for separation.

        // friction
        float friction;     ///< Friction of the constraint. yes.
        float frictionMass; ///< Mass to use for friction impulse calculation.
        /// TODO: store tangents?
        float frictionImpulse1; ///< Accumulated impulse for friction along the first tangent.
        float frictionImpulse2; ///< Accumulated impulse for friction along the second tangent.

        // restitution
        float restitution;      ///< Restitution coefficient of the constraint. yes.
        float relativeVelocity; ///< Relative velocity for computing restitution. This is to be removed (probably)

        std::vector<PenetrationConstraintPointData> points;

        // soft constraint
        float biasCoefficient;
        float impulseCoefficient;
        float massCoefficient;
    };
} // namespace cubos::engine
