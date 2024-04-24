#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/lights/point.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PointLight)
{
    return core::ecs::TypeBuilder<PointLight>("cubos::engine::PointLight")
        .withField("color", &PointLight::color)
        .withField("range", &PointLight::range)
        .withField("intensity", &PointLight::intensity)
        .build();
}
