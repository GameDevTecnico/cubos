#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/gizmos/target.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::GizmosTarget)
{
    return core::ecs::TypeBuilder<GizmosTarget>("cubos::engine::GizmosTarget").build();
}
