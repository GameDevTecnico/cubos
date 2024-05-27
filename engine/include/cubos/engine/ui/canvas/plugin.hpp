/// @dir
/// @brief @ref ui-canvas-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup ui-canvas-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup ui-canvas-plugin Canvas
    /// @ingroup ui-plugins
    /// @brief Adds the core UI functionality and rendering logic.
    ///
    /// # Architecture
    ///
    /// The two main UI components are @ref UIElement and @ref UICanvas.
    /// For it to work properly, each UIElement must be a child of a UICanvas, or of another UIElement.
    ///
    /// Each UIElement generates a list of drawing commands, @ref UIDrawList, that are then pooled together by the
    /// UICanvas at the root of the hierarchy.
    ///
    /// Each entry in a draw list contains a @ref UIDrawList::Type and a @ref UIDrawList::Command. The Type holds
    /// information that is common for all similar commands, and the Command contains the specifics to that one command.
    /// Each entry also has a raw data buffer to hold the data the command needs to pass to the shaders.
    ///
    /// After aggregating the draw commands, the UICanvas draws them, grouped by type.
    ///
    /// Check out @ref examples-engine-ui for the an example of the UI plugin in use.

    /// @brief @ref UICanvas components' information is passed to related @ref UIElement components.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiCanvasChildrenUpdateTag;

    /// @brief The @ref UIElement components are updated with information from their parents.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiElementPropagateTag;

    /// @brief Readies canvas for drawing.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiBeginTag;

    /// @brief Systems which draw to canvas should be tagged with this.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiDrawTag;

    /// @brief All queued draw commands are collected and executed to the render targets.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiEndTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup ui-canvas-plugin
    CUBOS_ENGINE_API void uiCanvasPlugin(Cubos& cubos);
} // namespace cubos::engine
