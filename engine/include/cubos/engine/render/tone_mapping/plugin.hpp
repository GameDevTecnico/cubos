/// @dir
/// @brief @ref render-tone-mapping-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-tone-mapping-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/tone_mapping/tone_mapping.hpp>

namespace cubos::engine
{
    /// @defgroup render-tone-mapping-plugin Tone Mapping
    /// @ingroup render-plugins
    /// @brief Draws the tone-mapped @ref HDR texture to the render target's framebuffer.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref shader-plugin
    /// - @ref render-target-plugin
    /// - @ref render-hdr-plugin

    /// @brief Tags the system which does the tone mapping.
    /// @ingroup render-tone-mapping-plugin
    CUBOS_ENGINE_API extern Tag toneMappingTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-tone-mapping-plugin
    CUBOS_ENGINE_API void toneMappingPlugin(Cubos& cubos);
} // namespace cubos::engine
