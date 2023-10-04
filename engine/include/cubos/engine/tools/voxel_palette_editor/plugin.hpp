/// @dir
/// @brief @ref voxel-palette-editor-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup voxel-palette-editor-tool-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @defgroup voxel-palette-editor-tool-plugin Palette editor
    /// @ingroup tool-plugins
    /// @brief Allows the user to open and inspect/edit a palette asset.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup voxel-palette-editor-tool-plugin
    void voxelPaletteEditorPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
