/// @dir
/// @brief @ref voxels-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup voxels-plugin Voxels
    /// @ingroup engine
    /// @brief Adds grid and palette assets to @b CUBOS.
    ///
    /// ## Bridges
    /// - @ref BinaryBridge - registered with the `.grd` extension, loads @ref Grid assets.
    /// - @ref BinaryBridge - registered with the `.pal` extension, loads @ref Palette assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup voxels-plugin
    void voxelsPlugin(Cubos& cubos);
} // namespace cubos::engine
