#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/collisions/broad_phase/potentially_colliding_with.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PotentiallyCollidingWith)
{
    return core::ecs::TypeBuilder<PotentiallyCollidingWith>("cubos::engine::PotentiallyCollidingWith")
        .symmetric()
        .build();
}