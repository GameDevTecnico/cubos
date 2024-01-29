/// @dir
/// @brief @ref imgui-plugin ImGui plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup imgui-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup imgui-plugin ImGui integration
    /// @ingroup engine
    /// @brief Initializes and configures ImGui for @b CUBOS.
    ///
    /// @note Systems using `ImGui` **must be** tagged with `cubos.imgui`.
    ///
    /// ## Resources
    /// - @ref DataInspector - used to inspect/modify objects via UI.
    ///
    /// ## Startup tags
    /// - `cubos.imgui.init` - `ImGui` is initialized, after `cubos.window.init`.
    ///
    /// ## Tags
    /// - `cubos.imgui.begin` - the ImGui frame begins (after `cubos.renderer.draw`).
    /// - `cubos.imgui.end` - the ImGui frame ends (before `cubos.window.render`).
    /// - `cubos.imgui` - runs between the previous two tags.
    ///
    /// ## Dependencies
    /// - @ref renderer-plugin
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup imgui-plugin
    void imguiPlugin(Cubos& cubos);
} // namespace cubos::engine
