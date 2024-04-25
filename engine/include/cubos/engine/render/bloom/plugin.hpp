/// @dir
/// @brief @ref render-bloom-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-bloom-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-bloom-plugin Bloom
    /// @ingroup render-plugins
    /// @brief Applies the Bloom effect on the @ref HDR texture.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref render-shader-plugin
    /// - @ref render-hdr-plugin

    /// @brief Tags the system which applies the Bloom effect.
    /// @ingroup render-bloom-plugin
    CUBOS_ENGINE_API extern Tag bloomTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-bloom-plugin
    CUBOS_ENGINE_API void bloomPlugin(Cubos& cubos);
} // namespace cubos::engine
