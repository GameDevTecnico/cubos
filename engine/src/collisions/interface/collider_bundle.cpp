#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/collider_bundle.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ColliderBundle)
{
    return core::ecs::TypeBuilder<ColliderBundle>("cubos::engine::ColliderBundle")
        .withField("isArea", &ColliderBundle::isArea)
        .withField("isStatic", &ColliderBundle::isStatic)
        .withField("isActive", &ColliderBundle::isActive)
        .withField("layers", &ColliderBundle::layers)
        .withField("mask", &ColliderBundle::mask)
        .build();
}