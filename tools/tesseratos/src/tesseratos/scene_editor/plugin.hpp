/// @dir
/// @brief @ref tesseratos-scene-editor-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-scene-editor-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/node.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-scene-editor-plugin Scene editor
    /// @ingroup tesseratos
    /// @brief Adds a window to edit scenes and select entities in them.
    ///
    /// @note Selected entities are registered in the @ref Selection resource.
    /// @note The opened scene is identified by the @ref AssetSelectedEvent event.

    /// @brief Tool state.
    struct SceneEditorTool
    {
        CUBOS_REFLECT;

        bool isOpen{false};

        /// @brief Root node of the current scene.
        cubos::engine::SceneNode sceneRoot;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-scene-editor-plugin
    void sceneEditorPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
