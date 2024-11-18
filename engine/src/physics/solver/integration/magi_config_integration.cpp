#include "magi_config_integration.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::MagiConfigIntegration)
{
    return core::ecs::TypeBuilder<MagiConfigIntegration>("cubos::engine::MagiConfigIntegration").build();
}