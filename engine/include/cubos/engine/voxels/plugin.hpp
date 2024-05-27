/// @dir
/// @brief @ref voxels-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup voxels-plugin Voxels
    /// @ingroup engine
    /// @brief Adds grid and palette assets to @b Cubos
    ///
    /// ## Bridges
    /// - @ref BinaryBridge - registered with the `.grd` extension, loads @ref VoxelGrid assets.
    /// - @ref BinaryBridge - registered with the `.pal` extension, loads @ref VoxelPalette assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup voxels-plugin
    CUBOS_ENGINE_API void voxelsPlugin(Cubos& cubos);
} // namespace cubos::engine
