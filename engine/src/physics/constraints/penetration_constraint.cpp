#include "penetration_constraint.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PenetrationConstraintPointData)
{
    return cubos::core::ecs::TypeBuilder<PenetrationConstraintPointData>(
               "cubos::engine::PenetrationConstraintPointData")
        .withField("initialSeparation", &PenetrationConstraintPointData::initialSeparation)
        .withField("normalSpeed", &PenetrationConstraintPointData::normalSpeed)
        .withField("localAnchor1", &PenetrationConstraintPointData::localAnchor1)
        .withField("localAnchor2", &PenetrationConstraintPointData::localAnchor2)
        .withField("fixedAnchor1", &PenetrationConstraintPointData::fixedAnchor1)
        .withField("fixedAnchor2", &PenetrationConstraintPointData::fixedAnchor2)
        .withField("normalMass", &PenetrationConstraintPointData::normalMass)
        .withField("normalImpulse", &PenetrationConstraintPointData::normalImpulse)
        .withField("frictionMass1", &PenetrationConstraintPointData::frictionMass1)
        .withField("frictionMass2", &PenetrationConstraintPointData::frictionMass2)
        .withField("frictionImpulse1", &PenetrationConstraintPointData::frictionImpulse1)
        .withField("frictionImpulse2", &PenetrationConstraintPointData::frictionImpulse2)
        .build();
}

CUBOS_REFLECT_IMPL(cubos::engine::PenetrationConstraint)
{
    return cubos::core::ecs::TypeBuilder<PenetrationConstraint>("cubos::engine::PenetrationConstraint")
        .withField("normal", &PenetrationConstraint::normal)
        .withField("friction", &PenetrationConstraint::friction)
        .withField("restitution", &PenetrationConstraint::restitution)
        .withField("points", &PenetrationConstraint::points)
        .withField("biasCoefficient", &PenetrationConstraint::biasCoefficient)
        .withField("impulseCoefficient", &PenetrationConstraint::impulseCoefficient)
        .withField("massCoefficient", &PenetrationConstraint::massCoefficient)
        .build();
}

CUBOS_REFLECT_IMPL(cubos::engine::PenetrationConstraints)
{
    return cubos::core::ecs::TypeBuilder<PenetrationConstraints>("cubos::engine::PenetrationConstraints")
        .symmetric()
        .withField("entity", &PenetrationConstraints::entity)
        .withField("penConstraints", &PenetrationConstraints::penConstraints)
        .build();
}
