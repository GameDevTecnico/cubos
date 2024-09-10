/// @dir
/// @brief @ref render-cascaded-shadow-maps-rasterizer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-cascaded-shadow-maps-rasterizer-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-cascaded-shadow-maps-rasterizer-plugin Cascaded shadow maps rasterizer
    /// @ingroup render-plugins
    /// @brief Draws all render meshes for each directional light to its shadow map.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref transform-plugin
    /// - @ref lights-plugin
    /// - @ref render-cascaded-shadow-maps-plugin
    /// - @ref render-shadows-plugin
    /// - @ref render-mesh-plugin
    /// - @ref render-voxels-plugin
    /// - @ref render-camera-plugin
    /// - @ref render-g-buffer-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-cascaded-shadow-maps-rasterizer-plugin
    CUBOS_ENGINE_API void cascadedShadowMapsRasterizerPlugin(Cubos& cubos);
} // namespace cubos::engine
