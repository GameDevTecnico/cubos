#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/child_of.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ChildOf)
{
    return core::ecs::TypeBuilder<ChildOf>("cubos::engine::ChildOf").tree().build();
}
