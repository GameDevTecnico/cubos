#include "physics_constants_integration.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PhysicsConstantsIntegration)
{
    return core::ecs::TypeBuilder<PhysicsConstantsIntegration>("cubos::engine::PhysicsConstantsIntegration").build();
}