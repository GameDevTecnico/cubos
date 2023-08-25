/// @file
/// @brief Classes @ref cubos::core::gl::SpotLight, @ref cubos::core::gl::DirectionalLight and @ref
/// cubos::core::gl::PointLight.
/// @ingroup core-gl

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::core::gl
{
    /// @brief Describes a spot light.
    /// @ingroup core-gl
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

    /// @brief Describes a directional light.
    /// @ingroup core-gl
    struct DirectionalLight
    {
        glm::quat rotation;
        glm::vec3 color;
        float intensity;

        DirectionalLight(const glm::quat& rotation, const glm::vec3& color, float intensity);
    };

    /// @brief Describes a point light.
    /// @ingroup core-gl
    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float range;

        PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range);
    };
} // namespace cubos::core::gl
