/// @dir
/// @brief @ref render-shadow-atlas-rasterizer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-shadow-atlas-rasterizer-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-shadow-atlas-rasterizer-plugin Shadow atlas rasterizer
    /// @ingroup render-plugins
    /// @brief Draws all render meshes for each light to the shadow map atlas.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref transform-plugin
    /// - @ref lights-plugin
    /// - @ref render-shadow-atlas-plugin
    /// - @ref render-shadows-plugin
    /// - @ref render-mesh-plugin
    /// - @ref render-voxels-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-shadow-atlas-rasterizer-plugin
    CUBOS_ENGINE_API void shadowAtlasRasterizerPlugin(Cubos& cubos);
} // namespace cubos::engine
