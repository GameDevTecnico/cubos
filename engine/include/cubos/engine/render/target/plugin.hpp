/// @dir
/// @brief @ref render-target-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-target-plugin

#pragma once

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
    extern Tag resizeRenderTargetTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-target-plugin
    void renderTargetPlugin(Cubos& cubos);
} // namespace cubos::engine
