/// @file
/// @brief Component @ref cubos::engine::DirectionalLight.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes an entity behave like a directional light.
    /// @note Should be used with @ref LocalToWorld.
    /// @todo In what direction does the light point for an identity transform?
    /// @ingroup renderer-plugin
    struct CUBOS_ENGINE_API DirectionalLight
    {
        CUBOS_REFLECT;

        glm::vec3 color = {1.0F, 1.0F, 1.0F}; ///< Color of the light.
        float intensity = 1.0F;               ///< Intensity of the light.
    };
} // namespace cubos::engine
