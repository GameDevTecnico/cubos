/// @file
/// @brief Resource @ref cubos::engine::RenderEnvironment.
/// @ingroup render-lights-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the scene's ambient light and sky colors.
    /// @ingroup render-lights-plugin
    struct CUBOS_ENGINE_API RenderEnvironment
    {
        CUBOS_REFLECT;

        /// @brief Ambient light color.
        glm::vec3 ambient = {0.2F, 0.2F, 0.2F};

        /// @brief Sky gradient color, where the first element specifies the sky color below, and
        /// the the second specifies the sky color above.
        glm::vec3 skyGradient[2] = {{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}};
    };
} // namespace cubos::engine
