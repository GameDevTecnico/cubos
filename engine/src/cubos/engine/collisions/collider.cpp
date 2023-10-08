#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/collisions/collider.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Collider)
{
    return core::ecs::ComponentTypeBuilder<Collider>("cubos::engine::Collider")
        .withField("transform", &Collider::transform)
        .build();
}
