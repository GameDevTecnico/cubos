#ifndef CUBOS_ENGINE_PLUGINS_IMGUI_HPP
#define CUBOS_ENGINE_PLUGINS_IMGUI_HPP

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin that adds ImGui functionalities and allows ImGui windows to be rendered.
    ///
    /// Startup Stages:
    /// - InitImGui: Initializes the UI window (after OpenWindow stage).
    ///
    /// Stages:
    /// - BeginImGuiFrame: Starts a new ImGui frame (after Poll stage).
    /// - EndImGuiFrame: Ends the ImGui frame (before SwapBuffers stage).
    ///
    /// If you want to render your own ImGui component system, the system should run
    /// between BeginImGuiFrame and EndImGuiFrame.
    /// @param cubos CUBOS. main class
    void imguiPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_IMGUI_HPP
