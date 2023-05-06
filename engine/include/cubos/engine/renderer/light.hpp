
#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// Component which makes an entity behave like a spot light.
    /// Should be used with `LocalToWorld`.
    struct [[cubos::component("cubos/spot_light", VecStorage)]] SpotLight
    {
        glm::vec3 color;
        float intensity;
        float range;
        float spotAngle;
        float innerSpotAngle;
    };

    /// Component which makes an entity behave like a directional light.
    /// Should be used with `LocalToWorld`.
    struct [[cubos::component("cubos/directional_light", VecStorage)]] DirectionalLight
    {
        glm::vec3 color;
        float intensity;
    };

    /// Component which makes an entity behave like a point light.
    /// Should be used with `LocalToWorld`.
    struct [[cubos::component("cubos/point_light", VecStorage)]] PointLight
    {
        glm::vec3 color;
        float intensity;
        float range;
    };
} // namespace cubos::engine
