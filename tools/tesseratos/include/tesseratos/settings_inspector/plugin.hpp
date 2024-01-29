/// @dir
/// @brief @ref tesseratos-settings-inspector-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-settings-inspector-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-settings-inspector-plugin Settings inspector
    /// @ingroup tesseratos
    /// @brief Allows inspecting the current setting values through a ImGui window.
    ///
    /// ## Dependencies
    /// - @ref settings-plugin
    /// - @ref imgui-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-settings-inspector-plugin
    void settingsInspectorPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
