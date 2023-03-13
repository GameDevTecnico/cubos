#ifndef CUBOS_ENGINE_PLUGINS_IMGUI_HPP
#define CUBOS_ENGINE_PLUGINS_IMGUI_HPP

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin to initialize and configure ImGui for CUBOS. Tag your ImGui systems with
    /// `cubos.imgui`.
    ///
    /// @details This plugin adds three systems: one to initialize ImGui, one to start a new
    /// ImGui frame, and one to end and render the ImGui frame. Between the two ImGui frame
    /// systems, you can add your own ImGui system, with the tag 'cubos.imgui'.
    ///
    /// Dependencies:
    /// - `windowPlugin`
    ///
    /// Startup tags:
    /// - `cubos.imgui.init`: initializes the ImGui, after `cubos.window.init`.
    ///
    /// Tags:
    /// - `cubos.imgui.begin`: starts a new ImGui frame (after `cubos.window.poll`).
    /// - `cubos.imgui.end`: ends the ImGui frame (before `cubos.window.render`).
    /// - `cubos.imgui`: runs your own ImGui system (between the previous two tags).
    ///
    /// @param cubos CUBOS. main class
    void imguiPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_IMGUI_HPP
