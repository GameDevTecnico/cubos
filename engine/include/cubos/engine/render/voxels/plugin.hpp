/// @dir
/// @brief @ref render-voxels-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-voxels-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-voxels-plugin Render Voxels
    /// @ingroup render-plugins
    /// @brief Common interface for voxel rendering plugins.
    ///
    /// By itself, this plugin does not provide any behavior. It simply registers the basic components which are then
    /// used by other plugins to render voxels. This allows for a modular design, where different rendering techniques
    /// can be implemented and swapped out without changing the rest of the engine.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-voxels-plugin
    CUBOS_ENGINE_API void renderVoxelsPlugin(Cubos& cubos);
} // namespace cubos::engine
