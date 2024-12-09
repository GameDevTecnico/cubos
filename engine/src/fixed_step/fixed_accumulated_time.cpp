#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/fixed_step/fixed_accumulated_time.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FixedAccumulatedTime)
{
    return core::ecs::TypeBuilder<FixedAccumulatedTime>("cubos::engine::FixedAccumulatedTime").build();
}
