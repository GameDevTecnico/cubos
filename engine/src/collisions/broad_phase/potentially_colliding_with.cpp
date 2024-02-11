#include "potentially_colliding_with.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PotentiallyCollidingWith)
{
    return core::ecs::TypeBuilder<PotentiallyCollidingWith>("cubos::engine::PotentiallyCollidingWith")
        .symmetric()
        .build();
}