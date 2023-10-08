#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/renderer/point_light.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PointLight)
{
    return core::ecs::ComponentTypeBuilder<PointLight>("cubos::engine::PointLight")
        .withField("color", &PointLight::color)
        .withField("range", &PointLight::range)
        .withField("intensity", &PointLight::intensity)
        .build();
}
