/// @file
/// @brief Component @ref cubos::engine::RenderDepth.
/// @ingroup render-depth-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the depth texture of a render target.
    /// @ingroup render-depth-plugin
    struct CUBOS_ENGINE_API RenderDepth
    {
        CUBOS_REFLECT;

        /// @brief Size of the depth texture, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Whether the depth texture has already been cleared this frame.
        bool cleared = false;

        /// @brief Handle to the actual depth texture.
        core::gl::Texture2D texture{nullptr};
    };
} // namespace cubos::engine
