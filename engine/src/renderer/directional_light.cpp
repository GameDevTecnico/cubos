#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/renderer/directional_light.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DirectionalLight)
{
    return core::ecs::TypeBuilder<DirectionalLight>("cubos::engine::DirectionalLight")
        .withField("color", &DirectionalLight::color)
        .withField("intensity", &DirectionalLight::intensity)
        .build();
}
