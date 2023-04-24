#include <cubos/core/gl/light.hpp>

using namespace cubos::core::gl;

SpotLight::SpotLight(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& color, float intensity,
                     float range, float spotAngle, float innerSpotAngle)
    : position(position)
    , rotation(rotation)
    , color(color)
    , intensity(intensity)
    , range(range)
    , spotAngle(spotAngle)
    , innerSpotAngle(innerSpotAngle)
{
}

DirectionalLight::DirectionalLight(const glm::quat& rotation, const glm::vec3& color, float intensity)
    : rotation(rotation)
    , color(color)
    , intensity(intensity)
{
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range)
    : position(position)
    , color(color)
    , intensity(intensity)
    , range(range)
{
}
