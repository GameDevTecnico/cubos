/// @file
/// @brief Components @ref cubos::engine::SpotLight, @ref cubos::engine::DirectionalLight and @ref
/// cubos::engine::PointLight.
/// @ingroup renderer-plugin
/// @todo Replace this file a directory with a file for each light type.

#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity emit a spot light.
    /// @note Should be used with @ref LocalToWorld.
    /// @todo In what direction does the spot light point for an identity transform?
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/spot_light", VecStorage)]] SpotLight
    {
        glm::vec3 color;
        float intensity;
        float range;
        float spotAngle;
    };

    /// @brief Component which makes an entity behave like a directional light.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/directional_light", VecStorage)]] DirectionalLight
    {
        glm::vec3 color;
        float intensity;
    };

    /// @brief Component which makes an entity behave like a point light.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/point_light", VecStorage)]] PointLight
    {
        glm::vec3 color;
        float intensity;
        float range;
    };
} // namespace cubos::engine
