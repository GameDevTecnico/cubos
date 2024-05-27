/// @dir
/// @brief @ref render-hdr-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-hdr-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-hdr-plugin HDR
    /// @ingroup render-plugins
    /// @brief Adds and manages @ref HDR components.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref render-target-plugin

    /// @brief Recreates the HDR texture if necessary - for example, due to a render target resize.
    /// @ingroup render-hdr-plugin
    CUBOS_ENGINE_API extern Tag createHDRTag;

    /// @brief Systems which draw to HDR textures should be tagged with this.
    /// @ingroup render-hdr-plugin
    CUBOS_ENGINE_API extern Tag drawToHDRTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-hdr-plugin
    CUBOS_ENGINE_API void hdrPlugin(Cubos& cubos);
} // namespace cubos::engine
