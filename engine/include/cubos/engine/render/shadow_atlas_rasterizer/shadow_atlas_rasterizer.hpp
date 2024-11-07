/// @file
/// @brief Resource @ref cubos::engine::ShadowAtlasRasterizer.
/// @ingroup render-shadow-atlas-rasterizer-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the ShadowAtlas rasterizer state.
    /// @ingroup render-shadow-atlas-rasterizer-plugin
    struct CUBOS_ENGINE_API ShadowAtlasRasterizer
    {
        CUBOS_REFLECT;

        /// @brief Framebuffer used by the rasterizer to render to the ShadowAtlas.
        core::gl::Framebuffer framebuffer{nullptr};

        /// @brief Framebuffer used by the rasterizer to render to the cube ShadowAtlas.
        core::gl::Framebuffer cubeFramebuffer{nullptr};

        /// @brief Atlas texture in the current framebuffer.
        core::gl::Texture2D atlas{nullptr};
    };
} // namespace cubos::engine
