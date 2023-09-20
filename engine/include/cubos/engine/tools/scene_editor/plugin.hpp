/// @dir
/// @brief @ref scene-editor-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup scene-editor-tool-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @defgroup scene-editor-tool-plugin Scene editor
    /// @ingroup tool-plugins
    /// @brief Adds a window to edit scenes and select entities in them.
    ///
    /// @note Selected entities are registered in the @ref EntitySelector resource.
    /// @note The opened scene is identified by the @ref AssetSelectedEvent event.
    ///
    /// ## Dependencies
    /// - @ref scene-plugin
    /// - @ref asset-explorer-tool-plugin
    /// - @ref entity-selector-tool-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup scene-editor-tool-plugin
    void sceneEditorPlugin(Cubos& cubos);
}; // namespace cubos::engine::tools