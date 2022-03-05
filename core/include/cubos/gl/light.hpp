
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
        glm::vec3 color;
        float intensity;
        float range;
        float spotAngle;
        float innerSpotAngle;
        bool castShadows;

        SpotLightData(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& color, float intensity,
                      float range, float spotAngle, float innerSpotAngle, bool castShadows);
        SpotLightData() = default;
    };

    struct DirectionalLightData
    {
        glm::quat rotation;
        glm::vec3 color;
        float intensity;
        bool castShadows;

        DirectionalLightData(const glm::quat& rotation, const glm::vec3& color, float intensity, bool castShadows);
        DirectionalLightData() = default;
    };

    struct PointLightData
    {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float range;
        bool castShadows;

        PointLightData(const glm::vec3& position, const glm::vec3& color, float intensity, float range,
                       bool castShadows);
        PointLightData() = default;
    };
} // namespace cubos::gl

#endif // CUBOS_GL_LIGHT_DATA_HPP
