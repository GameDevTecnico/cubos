/// @file
/// @brief Component @ref cubos::engine::GBufferRasterizer.
/// @ingroup render-g-buffer-rasterizer-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the GBuffer rasterizer state.
    /// @ingroup render-g-buffer-rasterizer-plugin
    struct CUBOS_ENGINE_API GBufferRasterizer
    {
        CUBOS_REFLECT;

        /// @brief Framebuffer used by the rasterizer to render to the GBuffer.
        core::gl::Framebuffer frontFramebuffer{nullptr};

        /// @brief Back framebuffer (swapped with front framebuffer every frame).
        ///
        /// This is necessary due to the double-buffering technique used by the @ref RenderPicker.
        core::gl::Framebuffer backFramebuffer{nullptr};

        /// @brief GBuffer's position texture in the current framebuffers.
        core::gl::Texture2D position{nullptr};

        /// @brief GBuffer's normal texture in the current framebuffers.
        core::gl::Texture2D normal{nullptr};

        /// @brief GBuffer's albedo texture in the current framebuffers.
        core::gl::Texture2D albedo{nullptr};

        /// @brief RenderDepth texture in the current framebuffers.
        core::gl::Texture2D depth{nullptr};

        /// @brief RenderPicker texture in the current front framebuffer.
        core::gl::Texture2D frontPicker{nullptr};

        /// @brief RenderPicker texture in the current back framebuffer.
        core::gl::Texture2D backPicker{nullptr};
    };
} // namespace cubos::engine
