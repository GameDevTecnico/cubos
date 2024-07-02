#include "penetration_constraint.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PenetrationConstraint)
{
    return cubos::core::ecs::TypeBuilder<PenetrationConstraint>("cubos::engine::PenetrationConstraint")
        .symmetric()
        .withField("entity", &PenetrationConstraint::entity)
        .withField("entity1OriginalPosition", &PenetrationConstraint::entity1OriginalPosition)
        .withField("entity2OriginalPosition", &PenetrationConstraint::entity2OriginalPosition)
        .withField("penetration", &PenetrationConstraint::penetration)
        .withField("normal", &PenetrationConstraint::normal)
        .withField("normalMass", &PenetrationConstraint::normalMass)
        .withField("normalImpulse", &PenetrationConstraint::normalImpulse)
        .withField("friction", &PenetrationConstraint::friction)
        .withField("frictionMass", &PenetrationConstraint::frictionMass)
        .withField("frictionImpulse1", &PenetrationConstraint::frictionImpulse1)
        .withField("frictionImpulse2", &PenetrationConstraint::frictionImpulse2)
        .withField("biasCoefficient", &PenetrationConstraint::biasCoefficient)
        .withField("impulseCoefficient", &PenetrationConstraint::impulseCoefficient)
        .withField("massCoefficient", &PenetrationConstraint::massCoefficient)
        .build();
}
