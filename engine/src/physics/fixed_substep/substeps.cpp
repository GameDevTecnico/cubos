#include "substeps.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Substeps)
{
    return core::ecs::TypeBuilder<Substeps>("cubos::engine::Substeps").wrap(&Substeps::value);
}
