/// @dir
/// @brief Settings inspector plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @defgroup settings-inspector-tool-plugin Settings inspector
    /// @ingroup tool-plugins
    /// @brief Allows inspecting the current setting values through a ImGui window.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class
    /// @ingroup settings-inspector-tool-plugin
    void settingsInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
