/// @dir
/// @brief @ref render-g-buffer-rasterizer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-g-buffer-rasterizer-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-g-buffer-rasterizer-plugin G-Buffer Rasterizer
    /// @ingroup render-plugins
    /// @brief Draws @ref RenderMesh components to the @ref GBuffer.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref transform-plugin
    /// - @ref shader-plugin
    /// - @ref render-g-buffer-plugin
    /// - @ref render-depth-plugin
    /// - @ref render-picker-plugin
    /// - @ref render-mesh-plugin
    /// - @ref render-voxels-plugin
    /// - @ref camera-plugin

    /// @brief Rasterizes @ref RenderMesh components to the GBuffer textures.
    /// @ingroup render-g-buffer-plugin
    CUBOS_ENGINE_API extern Tag rasterizeToGBufferTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-g-buffer-rasterizer-plugin
    CUBOS_ENGINE_API void gBufferRasterizerPlugin(Cubos& cubos);
} // namespace cubos::engine
