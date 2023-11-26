/// @file
/// @brief Component @ref cubos::engine::PointLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity behave like a point light.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct PointLight
    {
        CUBOS_REFLECT;

        glm::vec3 color = {1.0F, 1.0F, 1.0F}; ///< Color of the light.
        float intensity = 1.0F;               ///< Intensity of the light.
        float range = 5.0F;                   ///< Range of the light.
    };
} // namespace cubos::engine
