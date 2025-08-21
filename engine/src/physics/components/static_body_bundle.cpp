#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/physics/static_body_bundle.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::StaticBodyBundle)
{
    return cubos::core::ecs::TypeBuilder<StaticBodyBundle>("cubos::engine::StaticBodyBundle")
        .withField("isActive", &StaticBodyBundle::isActive)
        .withField("layers", &StaticBodyBundle::layers)
        .withField("mask", &StaticBodyBundle::mask)
        .withField("material", &StaticBodyBundle::material)
        .withField("centerOfMass", &StaticBodyBundle::centerOfMass)
        .build();
}
