/// @dir
/// @brief @ref render-ssao-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-ssao-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-ssao-plugin Screen Space Ambient Occlusion
    /// @ingroup render-plugins
    /// @brief Applies the SSAO technique on the @ref GBuffer and outputs the result to the @ref SSAO texture.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref render-shader-plugin
    /// - @ref render-g-buffer-plugin
    /// - @ref render-camera-plugin
    /// - @ref transform-plugin

    /// @brief Tags the system which applies the Screen Space Ambient Occlusion technique.
    /// @ingroup render-ssao-plugin
    CUBOS_ENGINE_API extern Tag drawToSSAOTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-ssao-plugin
    CUBOS_ENGINE_API void ssaoPlugin(Cubos& cubos);
} // namespace cubos::engine
