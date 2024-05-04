#include "substeps.hpp"

#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Substeps)
{
    return core::ecs::TypeBuilder<Substeps>("cubos::engine::Substeps").build();
}
