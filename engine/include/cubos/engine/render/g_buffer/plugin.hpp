/// @dir
/// @brief @ref render-g-buffer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-g-buffer-plugin

#pragma once

#include <cubos/engine/api.hpp>
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

    /// @brief Recreates the GBuffer if necessary - for example, due to a render target resize.
    /// @ingroup render-g-buffer-plugin
    CUBOS_ENGINE_API extern Tag createGBufferTag;

    /// @brief Systems which draw to GBuffer textures should be tagged with this.
    /// @ingroup render-g-buffer-plugin
    CUBOS_ENGINE_API extern Tag drawToGBufferTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-g-buffer-plugin
    CUBOS_ENGINE_API void gBufferPlugin(Cubos& cubos);
} // namespace cubos::engine
