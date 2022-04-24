
#ifndef CUBOS_CORE_GL_LIGHT_DATA_HPP
#define CUBOS_CORE_GL_LIGHT_DATA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::core::gl
{
    struct SpotLight
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 color;
        float intensity;
        float range;
        float spotAngle;
        float innerSpotAngle;

        SpotLight(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& color, float intensity,
                  float range, float spotAngle, float innerSpotAngle);
    };

    struct DirectionalLight
    {
        glm::quat rotation;
        glm::vec3 color;
        float intensity;

        DirectionalLight(const glm::quat& rotation, const glm::vec3& color, float intensity);
    };

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float range;

        PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range);
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_LIGHT_DATA_HPP
