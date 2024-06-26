/// @dir
/// @brief @ref render-depth-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-depth-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-depth-plugin Depth
    /// @ingroup render-plugins
    /// @brief Adds and manages the @ref RenderDepth component.
    ///
    /// ## Components
    /// - @ref RenderDepth
    ///
    /// ## Dependencies
    /// - @ref render-target-plugin
    /// - @ref window-plugin

    /// @brief Recreates the RenderDepth texture if necessary - for example, due to a render target resize.
    /// @ingroup render-depth-plugin
    CUBOS_ENGINE_API extern Tag createRenderDepthTag;

    /// @brief Systems which draw to RenderDepth textures should be tagged with this.
    /// @ingroup render-depth-plugin
    CUBOS_ENGINE_API extern Tag drawToRenderDepthTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-depth-plugin
    CUBOS_ENGINE_API void renderDepthPlugin(Cubos& cubos);
} // namespace cubos::engine
