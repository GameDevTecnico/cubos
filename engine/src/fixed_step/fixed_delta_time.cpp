#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/fixed_step/fixed_delta_time.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FixedDeltaTime)
{
    return core::ecs::TypeBuilder<FixedDeltaTime>("cubos::engine::FixedDeltaTime").build();
}
