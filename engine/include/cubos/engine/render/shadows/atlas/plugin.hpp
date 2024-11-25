/// @dir
/// @brief @ref render-shadow-atlas-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-shadow-atlas-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-shadow-atlas-plugin Shadow atlas
    /// @ingroup render-shadows-plugins
    /// @brief Creates and manages shadow map atlases.
    ///
    /// ## Dependencies
    /// - @ref render-shadow-casters-plugin
    /// - @ref window-plugin

    /// @brief Creates the shadow atlases.
    /// @ingroup render-shadow-atlas-plugin
    CUBOS_ENGINE_API extern Tag createShadowAtlasTag;

    /// @brief Reserves space for shadow casters.
    /// @ingroup render-shadow-atlas-plugin
    CUBOS_ENGINE_API extern Tag reserveShadowCastersTag;

    /// @brief Systems which draw to the shadow atlas textures should be tagged with this.
    /// @ingroup render-shadow-atlas-plugin
    CUBOS_ENGINE_API extern Tag drawToShadowAtlasTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-shadow-atlas-plugin
    CUBOS_ENGINE_API void shadowAtlasPlugin(Cubos& cubos);
} // namespace cubos::engine
