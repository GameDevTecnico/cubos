/// @dir
/// @brief ImGui plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup imgui-plugin ImGui integration
    /// @ingroup plugins
    /// @brief Initializes and configures ImGui for CUBOS.
    ///
    /// @note Systems using `ImGui` **must be** tagged with `cubos.imgui`.
    ///
    /// ## Startup tags
    /// - `cubos.imgui.init` - `ImGui` is initialized, after `cubos.window.init`.
    ///
    /// ## Tags
    /// - `cubos.imgui.begin` - the ImGui frame begins (after `cubos.window.poll`).
    /// - `cubos.imgui.end` - the ImGui frame ends (before `cubos.window.render`).
    /// - `cubos.imgui` - runs between the previous two tags.
    ///
    /// ## Dependencies
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class
    /// @ingroup imgui-plugin
    void imguiPlugin(Cubos& cubos);
} // namespace cubos::engine
