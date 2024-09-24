/// @dir
/// @brief @ref settings-inspector-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup settings-inspector-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup settings-inspector-tool-plugin Settings Inspector
    /// @ingroup tool-plugins
    /// @brief Allows inspecting the current setting values through a ImGui window.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup settings-inspector-tool-plugin
    void settingsInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine
