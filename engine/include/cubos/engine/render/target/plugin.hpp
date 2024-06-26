/// @dir
/// @brief @ref render-target-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-target-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-target-plugin Render Target
    /// @ingroup render-plugins
    /// @brief Adds and manages @ref RenderTarget components.
    ///
    /// ## Dependencies
    /// - @ref window-plugin

    /// @brief Resizes the Render Target if necessary - for example, due to a window resize.
    /// @ingroup render-target-plugin
    CUBOS_ENGINE_API extern Tag resizeRenderTargetTag;

    /// @brief Systems which draw to Render Target framebuffers should be tagged with this.
    /// @ingroup render-target-plugin
    CUBOS_ENGINE_API extern Tag drawToRenderTargetTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-target-plugin
    CUBOS_ENGINE_API void renderTargetPlugin(Cubos& cubos);
} // namespace cubos::engine
