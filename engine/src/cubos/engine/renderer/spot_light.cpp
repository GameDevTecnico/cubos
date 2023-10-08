#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/renderer/spot_light.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SpotLight)
{
    return core::ecs::ComponentTypeBuilder<SpotLight>("cubos::engine::SpotLight")
        .withField("color", &SpotLight::color)
        .withField("intensity", &SpotLight::intensity)
        .withField("range", &SpotLight::range)
        .withField("spotAngle", &SpotLight::spotAngle)
        .build();
}
