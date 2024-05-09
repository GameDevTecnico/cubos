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
    /// @ingroup engine
    /// @brief Adds UI canvas and elements.

    /// @brief @ref UICanvas components' information is passed to related @ref UIElement components.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag elementVPUpdateTag;

    /// @brief The @ref UIElement components are updated with information from their parents.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag elementPropagateTag;

    /// @brief Readies canvas for drawing.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiBeginTag;

    /// @brief Systems which draw to canvas should be tagged with this.
    /// @ingroup canvas-target-plugin
    CUBOS_ENGINE_API extern Tag uiDrawTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup ui-canvas-plugin
    CUBOS_ENGINE_API void uiCanvasPlugin(Cubos& cubos);
} // namespace cubos::engine
