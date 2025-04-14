#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/physics/constraints/distance_constraint.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DistanceConstraint)
{
    return cubos::core::ecs::TypeBuilder<DistanceConstraint>("cubos::engine::DistanceConstraint")
        .withField("isRigid", &DistanceConstraint::isRigid)
        .withField("fixedDistance", &DistanceConstraint::fixedDistance)
        .withField("minDistance", &DistanceConstraint::minDistance)
        .withField("maxDistance", &DistanceConstraint::maxDistance)
        .withField("deltaCenter", &DistanceConstraint::deltaCenter)
        .withField("localAnchor1", &DistanceConstraint::localAnchor1)
        .withField("localAnchor2", &DistanceConstraint::localAnchor2)
        .withField("axialMass", &DistanceConstraint::axialMass)
        .withField("impulse", &DistanceConstraint::impulse)
        .withField("lowerImpulse", &DistanceConstraint::lowerImpulse)
        .withField("upperImpulse", &DistanceConstraint::upperImpulse)
        .withField("biasCoefficient", &DistanceConstraint::biasCoefficient)
        .withField("impulseCoefficient", &DistanceConstraint::impulseCoefficient)
        .withField("massCoefficient", &DistanceConstraint::massCoefficient)
        .build();
}