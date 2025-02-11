#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CollidingWith)
{
    return core::ecs::TypeBuilder<CollidingWith>("cubos::engine::CollidingWith")
        .symmetric()
        .withField("entity", &CollidingWith::entity)
        .withField("manifolds", &CollidingWith::manifolds)
        .build();
}