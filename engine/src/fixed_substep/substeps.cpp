#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/fixed_substep/substeps.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Substeps)
{
    return core::ecs::TypeBuilder<Substeps>("cubos::engine::Substeps").build();
}
