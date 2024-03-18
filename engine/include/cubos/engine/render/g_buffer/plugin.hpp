/// @dir
/// @brief @ref render-g-buffer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-g-buffer-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-g-buffer-plugin G-Buffer
    /// @ingroup render-plugins
    /// @brief Adds and manages @ref GBuffer components.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref render-target-plugin

    // TODO move this to render target plugin when that's done
    extern Tag resizeRenderTargetTag;

    /// @brief Recreates the GBuffer if necessary - for example, due to a render target resize.
    extern Tag createGBufferTag;

    /// @brief Systems which draw to GBuffer textures should be tagged with this.
    extern Tag drawToGBufferTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-g-buffer-plugin
    void gBufferPlugin(Cubos& cubos);
} // namespace cubos::engine
