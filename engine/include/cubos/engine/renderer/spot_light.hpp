/// @file
/// @brief Component @ref cubos::engine::SpotLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

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
} // namespace cubos::engine
