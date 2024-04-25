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

        /// @brief Size of the HDR textures, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Handle to the HDR texture to write to/read from.
        core::gl::Texture2D frontTexture{nullptr};

        /// @brief Handle to the HDR texture to be written to by post-processing plugins.
        ///
        /// Each effect should write to this texture and then swap it with @ref frontTexture.
        core::gl::Texture2D backTexture{nullptr};
    };
} // namespace cubos::engine
