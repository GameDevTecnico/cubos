/// @dir
/// @brief @ref render-cascaded-shadow-maps-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-cascaded-shadow-maps-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-cascaded-shadow-maps-plugin Cascaded shadow maps
    /// @ingroup render-shadows-plugins
    /// @brief Creates and manages shadow maps for directional shadow casters.
    ///
    /// ## Dependencies
    /// - @ref render-camera-plugin
    /// - @ref render-shadow-casters-plugin
    /// - @ref window-plugin

    /// @brief Creates the shadow maps.
    /// @ingroup render-cascaded-shadow-maps-plugin
    CUBOS_ENGINE_API extern Tag createCascadedShadowMapsTag;

    /// @brief Systems which draw to the cascaded shadow maps should be tagged with this.
    /// @ingroup render-cascaded-shadow-maps-plugin
    CUBOS_ENGINE_API extern Tag drawToCascadedShadowMapsTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-cascaded-shadow-maps-plugin
    CUBOS_ENGINE_API void cascadedShadowMapsPlugin(Cubos& cubos);
} // namespace cubos::engine
