#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/engine/collisions/shapes/box.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::BoxCollisionShape)
{
    return core::ecs::ComponentTypeBuilder<BoxCollisionShape>("cubos::engine::BoxCollisionShape")
        .withField("box", &BoxCollisionShape::box)
        .build();
}
