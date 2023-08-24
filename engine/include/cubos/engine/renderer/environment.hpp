/// @file
/// @brief Resource @ref cubos::engine::RendererEnvironment.
/// @ingroup renderer-plugin

#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the renderer's ambient light and sky colors.
    /// @ingroup renderer-plugin
    struct RendererEnvironment
    {
        /// @brief Ambient light color.
        glm::vec3 ambient = {0.2F, 0.2F, 0.2F};

        /// @brief Sky gradient color, where the first element specifies the sky color below, and
        /// the the second specifies the sky color above.
        glm::vec3 skyGradient[2] = {{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}};
    };
} // namespace cubos::engine
