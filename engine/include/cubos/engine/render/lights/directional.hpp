/// @file
/// @brief Component @ref cubos::engine::DirectionalLight.
/// @ingroup render-lights-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity behave like a directional light.
    ///
    /// The light points towards the local forward vector of the entity.
    ///
    /// @ingroup render-lights-plugin
    struct CUBOS_ENGINE_API DirectionalLight
    {
        CUBOS_REFLECT;

        glm::vec3 color = {1.0F, 1.0F, 1.0F}; ///< Color of the light.
        float intensity = 1.0F;               ///< Intensity of the light.
    };
} // namespace cubos::engine
