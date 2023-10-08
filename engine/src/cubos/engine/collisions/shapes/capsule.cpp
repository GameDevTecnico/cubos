#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/engine/collisions/shapes/capsule.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CapsuleCollisionShape)
{
    return core::ecs::ComponentTypeBuilder<CapsuleCollisionShape>("cubos::engine::CapsuleCollisionShape")
        .withField("capsule", &CapsuleCollisionShape::capsule)
        .build();
}
