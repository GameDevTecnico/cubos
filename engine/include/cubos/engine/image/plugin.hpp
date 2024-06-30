/// @dir
/// @brief @ref image-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup image-plugin

#pragma once

#include <cubos/engine/image/image.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup image-plugin Image
    /// @ingroup engine
    /// @brief Adds images to @b Cubos using stb_image.
    ///
    /// ## Bridges
    /// - @ref ImageBridge - registered with the `.png` extension, loads @ref Image assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup image-plugin
    CUBOS_ENGINE_API void imagePlugin(Cubos& cubos);
} // namespace cubos::engine