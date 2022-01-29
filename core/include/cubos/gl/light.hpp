
#ifndef CUBOS_GL_LIGHT_DATA_HPP
#define CUBOS_GL_LIGHT_DATA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::gl
{
    struct SpotLightData
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec4 color;
        float intensity;
        float range;
        float spotAngle;
        float innerSpotAngle;
    };

    struct DirectionalLightData
    {
        glm::quat rotation;
        glm::vec4 color;
        float intensity;
    };

    struct PointLightData
    {
        glm::vec3 position;
        glm::vec4 color;
        float intensity;
        float range;
    };
} // namespace cubos::gl

#endif // CUBOS_GL_LIGHT_DATA_HPP
