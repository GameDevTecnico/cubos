#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/physics/constraints/spring_constraint.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SpringConstraint)
{
    return cubos::core::ecs::TypeBuilder<SpringConstraint>("cubos::engine::SpringConstraint")
        .withField("stiffness", &SpringConstraint::stiffness)
        .withField("damping", &SpringConstraint::damping)
        .withField("restLength", &SpringConstraint::restLength)
        .withField("localAnchor1", &SpringConstraint::localAnchor1)
        .withField("localAnchor2", &SpringConstraint::localAnchor2)
        .build();
}
