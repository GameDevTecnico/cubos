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
    /// @brief Holds the data for a contact point of the @ref PenetrationContraint.
    /// @ingroup physics-solver-plugin
    struct PenetrationConstraintPointData
    {
        CUBOS_REFLECT;

        /// TODO: change to initialSeparation
        float initialSeparation; ///< The separation of the contact point. Negative separation indicates
                                 ///< penetration.
        float normalSpeed; ///< The relative velocity of the bodies along the normal at the contact point the begging of
                           ///< the collision.

        glm::vec3 localAnchor1; ///< The local contact point relative to the center of mass of the first body.
        glm::vec3 localAnchor2; ///< The local contact point relative to the center of mass of the second body.

        /// Store fixed world-space anchors.
        /// This improves rolling behavior for shapes like balls and capsules. Used for restitution and friction.
        glm::vec3 fixedAnchor1; ///< The world-space contact point relative to the center of mass of the first body.
        glm::vec3 fixedAnchor2; ///< The world-space contact point relative to the center of mass of the second body.

        // separation
        float normalMass;    ///< Mass to use for normal impulse calculation.
        float normalImpulse; ///< Accumulated impulse for separation.

        // friction
        float frictionMass1;    ///< Mass to use for friction impulse calculation along the first tangent..
        float frictionMass2;    ///< Mass to use for friction impulse calculation along the second tangent..
        float frictionImpulse1; ///< Accumulated impulse for friction along the first tangent.
        float frictionImpulse2; ///< Accumulated impulse for friction along the second tangent.
    };

    /// @brief Relation which holds the information for resolving a penetration between particles.
    /// @ingroup physics-solver-plugin
    struct PenetrationConstraint
    {
        CUBOS_REFLECT;

        glm::vec3 normal;                ///< Normal of contact on the surface of the entity.
        float friction;                  ///< Friction of the constraint.
        float restitution;               ///< Restitution coefficient of the constraint.

        std::vector<PenetrationConstraintPointData> points; ///< Contact points in the contact manifold.

        // soft constraint
        float biasCoefficient;
        float impulseCoefficient;
        float massCoefficient;
    };

    /// @brief Holds multiple PenetrationConstraints
    /// @ingroup physics-solver-plugin
    struct PenetrationConstraints
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to.

        std::vector<PenetrationConstraint> penConstraints; ///< Penetration contraints for each manifold.
    };
} // namespace cubos::engine
