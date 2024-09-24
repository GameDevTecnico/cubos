/// @dir
/// @brief @ref selection-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup selection-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/tools/selection/selection.hpp>

namespace cubos::engine
{
    /// @defgroup selection-tool-plugin Selection
    /// @ingroup tool-plugins
    /// @brief Adds a resource which keeps track of what the current selection is.
    ///
    /// This plugins exists to reduce coupling between plugins. For example, a plugin which allows
    /// selecting entities through a ImGui window only needs to depend on this plugin, instead of
    /// having to know about all the plugins which care about it. The same applies in the other
    /// direction.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup selection-tool-plugin
    void selectionPlugin(Cubos& cubos);
} // namespace cubos::engine
