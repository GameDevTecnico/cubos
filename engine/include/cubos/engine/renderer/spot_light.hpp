/// @file
/// @brief Component @ref cubos::engine::SpotLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity emit a spot light in the direction of its forward vector.
    /// @ingroup renderer-plugin
    struct SpotLight
    {
        CUBOS_REFLECT;

        glm::vec3 color = {1.0F, 1.0F, 1.0F}; ///< Color of the light.
        float intensity = 1.0F;               ///< Intensity of the light.
        float range = 5.0F;                   ///< Range of the light.
        float spotAngle = 60.0F;              ///< Angle of the spot light in degrees.
        float innerSpotAngle = 30.0F;         ///< Angle of the spot light in degrees.
    };
} // namespace cubos::engine
