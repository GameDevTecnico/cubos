#include <cubos/gl/light.hpp>

using namespace cubos::gl;

SpotLightData::SpotLightData(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& color,
                             float intensity, float range, float spotAngle, float innerSpotAngle)
    : position(position), rotation(rotation), color(color), intensity(intensity), range(range), spotAngle(spotAngle),
      innerSpotAngle(innerSpotAngle)
{
}

DirectionalLightData::DirectionalLightData(const glm::quat& rotation, const glm::vec3& color, float intensity)
    : rotation(rotation), color(color), intensity(intensity)
{
}

PointLightData::PointLightData(const glm::vec3& position, const glm::vec3& color, float intensity, float range)
    : position(position), color(color), intensity(intensity), range(range)
{
}
