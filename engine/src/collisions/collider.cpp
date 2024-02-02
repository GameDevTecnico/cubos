#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/collisions/collider.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Collider)
{
    return core::ecs::TypeBuilder<Collider>("cubos::engine::Collider")
        .withField("transform", &Collider::transform)
        .build();
}
