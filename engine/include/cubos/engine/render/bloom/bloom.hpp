/// @file
/// @brief Component @ref cubos::engine::Bloom.
/// @ingroup render-bloom-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the Bloom state for a render target.
    /// @ingroup render-bloom-plugin
    struct CUBOS_ENGINE_API Bloom
    {
        CUBOS_REFLECT;

        /// @brief Number of blur passes to apply.
        int iterations = 5;

        /// @brief Threshold for the Bloom effect.
        float threshold = 1.0F;

        /// @brief Soft threshold for the Bloom effect.
        float softThreshold = 0.5F;

        /// @brief Intensity of the Bloom effect.
        float intensity = 1.0F;

        /// @brief Luminance values used to calculate the Bloom effect.
        glm::vec3 luminance{0.2126F, 0.7152F, 0.0722F};

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

        /// @brief Size of the textures.
        glm::uvec2 size{};

        /// @brief Texture where the Bloom extraction result is stored.
        core::gl::Texture2D extractionTexture;

        /// @brief Framebuffer used to draw to the Bloom extraction texture.
        core::gl::Framebuffer extractionFramebuffer;

        /// @brief Textures where the Bloom result is stored.
        std::vector<core::gl::Texture2D> bloomTextures;

        /// @brief Framebuffers used to draw to the Bloom textures.
        std::vector<core::gl::Framebuffer> bloomFramebuffers;
    };
} // namespace cubos::engine
