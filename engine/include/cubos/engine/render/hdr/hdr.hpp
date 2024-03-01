/// @file
/// @brief Component @ref cubos::engine::HDR.
/// @ingroup render-hdr-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the HDR texture of a render target.
    /// @ingroup render-hdr-plugin
    struct CUBOS_ENGINE_API HDR
    {
        CUBOS_REFLECT;

        /// @brief Size of the HDR texture, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Handle to the actual HDR texture.
        core::gl::Texture2D texture{nullptr};
    };
} // namespace cubos::engine
