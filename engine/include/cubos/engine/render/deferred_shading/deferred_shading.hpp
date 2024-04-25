/// @file
/// @brief Component @ref cubos::engine::DeferredShading.
/// @ingroup render-deferred-shading-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the Deferred Shading state for a render target.
    /// @ingroup render-deferred-shading-plugin
    struct CUBOS_ENGINE_API DeferredShading
    {
        CUBOS_REFLECT;

        /// @brief Framebuffer used to draw to the HDR texture.
        core::gl::Framebuffer frontFramebuffer;

        /// @brief Framebuffer used to draw to the back HDR texture.
        ///
        /// Swapped with @ref frontFramebuffer as needed so that the textures match with the @ref HDR component.
        core::gl::Framebuffer backFramebuffer;

        /// @brief HDR texture present in the @ref frontFramebuffer.
        core::gl::Texture2D frontHDR;

        /// @brief HDR texture present in the @ref backFramebuffer.
        core::gl::Texture2D backHDR;
    };
} // namespace cubos::engine
