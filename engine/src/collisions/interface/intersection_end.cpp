#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/collisions/intersection_end.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::IntersectionEnd)
{
    return core::ecs::TypeBuilder<IntersectionEnd>("cubos::engine::IntersectionEnd").symmetric().build();
}