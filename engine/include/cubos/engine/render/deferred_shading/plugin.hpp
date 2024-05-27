/// @dir
/// @brief @ref render-deferred-shading-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-deferred-shading-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-deferred-shading-plugin Deferred Shading
    /// @ingroup render-plugins
    /// @brief Applies the Deferred Shading technique on the @ref GBuffer and outputs the result to the @ref HDR
    /// texture.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref render-shader-plugin
    /// - @ref render-g-buffer-plugin
    /// - @ref render-ssao-plugin
    /// - @ref render-hdr-plugin
    /// - @ref render-camera-plugin
    /// - @ref render-lights-plugin
    /// - @ref transform-plugin

    /// @brief Tags the system which applies the Deferred Shading technique.
    /// @ingroup render-deferred-shading-plugin
    CUBOS_ENGINE_API extern Tag deferredShadingTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-deferred-shading-plugin
    CUBOS_ENGINE_API void deferredShadingPlugin(Cubos& cubos);
} // namespace cubos::engine
