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
    /// ## Dependencies
    /// - @ref renderer-plugin
    /// - @ref window-plugin

    /// @brief `ImGui` is initialized, after `cubos.window.init`.
    extern Tag ImguiInitTag;

    /// @brief the ImGui frame begins (after `cubos.renderer.draw`).
    extern Tag ImguiBeginTag;

    /// @brief the ImGui frame ends (before `cubos.window.render`).
    extern Tag ImguiEndTag;

    /// @brief runs between the previous two tags.
    extern Tag ImguiTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup imgui-plugin
    void imguiPlugin(Cubos& cubos);
} // namespace cubos::engine
