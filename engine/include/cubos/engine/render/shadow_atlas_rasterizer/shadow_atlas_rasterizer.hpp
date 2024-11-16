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

        /// @brief Framebuffer used by the rasterizer to render to the spot shadow atlas.
        core::gl::Framebuffer spotAtlasFramebuffer{nullptr};

        /// @brief Framebuffer used by the rasterizer to render to the point shadow atlas.
        core::gl::Framebuffer pointAtlasFramebuffer{nullptr};

        /// @brief Atlas texture stored to check if the framebuffers need to be recreated.
        core::gl::Texture2D atlas{nullptr};
    };
} // namespace cubos::engine
