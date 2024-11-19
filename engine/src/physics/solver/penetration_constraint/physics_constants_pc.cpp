#include "physics_constants_pc.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PhysicsConstantsPC)
{
    return core::ecs::TypeBuilder<PhysicsConstantsPC>("cubos::engine::PhysicsConstantsPC")
        .withField("biasMax", &PhysicsConstantsPC::biasMax)
        .withField("contactHertzMin", &PhysicsConstantsPC::contactHertzMin)
        .withField("kNormal", &PhysicsConstantsPC::kNormal)
        .withField("kFriction", &PhysicsConstantsPC::kFriction)
        .build();
}