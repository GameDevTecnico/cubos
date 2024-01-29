/// @dir
/// @brief @ref tesseratos-voxel-palette-editor-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-voxel-palette-editor-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-voxel-palette-editor-plugin Palette editor
    /// @ingroup tesseratos
    /// @brief Allows the user to open and inspect/edit a palette asset.
    ///
    /// ## Dependencies
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-voxel-palette-editor-plugin
    void voxelPaletteEditorPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
