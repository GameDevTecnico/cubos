/// @dir
/// @brief @ref tesseratos-scene-editor-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-scene-editor-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-scene-editor-plugin Scene editor
    /// @ingroup tesseratos
    /// @brief Adds a window to edit scenes and select entities in them.
    ///
    /// @note Selected entities are registered in the @ref EntitySelector resource.
    /// @note The opened scene is identified by the @ref AssetSelectedEvent event.
    ///
    /// ## Dependencies
    /// - @ref scene-plugin
    /// - @ref tesseratos-asset-explorer-plugin
    /// - @ref tesseratos-entity-selector-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-scene-editor-plugin
    void sceneEditorPlugin(cubos::engine::Cubos& cubos);
}; // namespace tesseratos