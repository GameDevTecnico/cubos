#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/collisions/intersection_start.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::IntersectionStart)
{
    return core::ecs::TypeBuilder<IntersectionStart>("cubos::engine::IntersectionStart").symmetric().build();
}