/// @dir
/// @brief @ref settings-inspector-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup settings-inspector-tool-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @defgroup settings-inspector-tool-plugin Settings inspector
    /// @ingroup tool-plugins
    /// @brief Allows inspecting the current setting values through a ImGui window.
    ///
    /// ## Dependencies
    /// - @ref settings-plugin
    /// - @ref imgui-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup settings-inspector-tool-plugin
    void settingsInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
