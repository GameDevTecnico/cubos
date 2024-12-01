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

        /// @brief Framebuffers used by the rasterizer to render to each face of the point shadow atlas.
        core::gl::Framebuffer pointAtlasFramebuffer[6]{};

        /// @brief Atlas texture stored to check if the spot atlas framebuffer needs to be recreated.
        core::gl::Texture2D spotAtlas{nullptr};

        /// @brief Atlas texture stored to check if the point atlas framebuffer needs to be recreated.
        core::gl::Texture2DArray pointAtlas{nullptr};
    };
} // namespace cubos::engine
