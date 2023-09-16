/// @file
/// @brief Component @ref cubos::engine::DirectionalLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity behave like a directional light.
    /// @note Should be used with @ref LocalToWorld.
    /// @todo In what direction does the light point for an identity transform?
    /// @ingroup renderer-plugin
    struct [[cubos::component("cubos/directional_light", VecStorage)]] DirectionalLight
    {
        glm::vec3 color;
        float intensity;
    };
} // namespace cubos::engine
