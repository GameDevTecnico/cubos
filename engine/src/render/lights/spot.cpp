#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/lights/spot.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SpotLight)
{
    return core::ecs::TypeBuilder<SpotLight>("cubos::engine::SpotLight")
        .withField("color", &SpotLight::color)
        .withField("intensity", &SpotLight::intensity)
        .withField("range", &SpotLight::range)
        .withField("spotAngle", &SpotLight::spotAngle)
        .withField("innerSpotAngle", &SpotLight::innerSpotAngle)
        .build();
}
