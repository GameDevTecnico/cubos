/// @dir
/// @brief @ref ui-text-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup ui-text-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup ui-text-plugin
    /// @ingroup ui-plugins
    /// @brief Adds text element to UI.
    ///
    /// ## Dependencies:
    /// - @ref ui-canvas-plugin
    /// - @ref window-plugin
    /// - @ref render-shader-plugin
    /// - @ref assets-plugin
    /// - @ref font-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup ui-text-plugin
    CUBOS_ENGINE_API void uiTextPlugin(Cubos& cubos);

} // namespace cubos::engine
