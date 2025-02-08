#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/collisions/shapes/capsule.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CapsuleCollisionShape)
{
    return core::ecs::TypeBuilder<CapsuleCollisionShape>("cubos::engine::CapsuleCollisionShape")
        .wrap(&CapsuleCollisionShape::capsule);
}
