/// @file
/// @brief Component @ref cubos::engine::PointLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

namespace cubos::engine
{
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
