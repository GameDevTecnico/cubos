/// @file
/// @brief Component @ref cubos::engine::SSAO.
/// @ingroup render-ssao-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the SSAO textures for a render target.
    /// @ingroup render-ssao-plugin
    struct CUBOS_ENGINE_API SSAO
    {
        CUBOS_REFLECT;

        /// @brief Size of the SSAO textures, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Number of samples to use.
        int samples = 64;

        /// @brief Radius parameter.
        float radius = 0.5F;

        /// @brief Bias parameter.
        float bias = 0.025F;

        /// @brief Framebuffer used to draw to the base SSAO texture.
        core::gl::Framebuffer baseFramebuffer;

        /// @brief Framebuffer used to draw to the base SSAO texture.
        core::gl::Framebuffer blurFramebuffer;

        /// @brief SSAO base texture.
        core::gl::Texture2D baseTexture;

        /// @brief SSAO blurred texture.
        core::gl::Texture2D blurTexture;
    };
} // namespace cubos::engine
