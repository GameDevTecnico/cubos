#include "accumulated_correction.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AccumulatedCorrection)
{
    return cubos::core::ecs::TypeBuilder<AccumulatedCorrection>("cubos::engine::AccumulatedCorrection")
        .wrap(&AccumulatedCorrection::position);
}
