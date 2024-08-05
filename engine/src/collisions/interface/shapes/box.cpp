#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/shapes/box.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::BoxCollisionShape)
{
    return core::ecs::TypeBuilder<BoxCollisionShape>("cubos::engine::BoxCollisionShape")
        .withField("box", &BoxCollisionShape::box)
        .withField("changed", &BoxCollisionShape::changed)
        .build();
}
