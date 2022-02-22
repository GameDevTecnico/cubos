#include <cubos/gl/light.hpp>

using namespace cubos::gl;

SpotLightData::SpotLightData(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& color,
                             float intensity, float range, float spotAngle, float innerSpotAngle, bool castShadows)
    : position(position), rotation(rotation), color(color), intensity(intensity), range(range), spotAngle(spotAngle),
      innerSpotAngle(innerSpotAngle), castShadows(castShadows)
{
}

DirectionalLightData::DirectionalLightData(const glm::quat& rotation, const glm::vec3& color, float intensity,
                                           bool castShadows)
    : rotation(rotation), color(color), intensity(intensity), castShadows(castShadows)
{
}

PointLightData::PointLightData(const glm::vec3& position, const glm::vec3& color, float intensity, float range,
                               bool castShadows)
    : position(position), color(color), intensity(intensity), range(range), castShadows(castShadows)
{
}
