#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/accumulated_correction.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AccumulatedCorrection)
{
    return cubos::core::ecs::TypeBuilder<AccumulatedCorrection>("cubos::engine::AccumulatedCorrection")
        .withField("position", &AccumulatedCorrection::position)
        .build();
}
