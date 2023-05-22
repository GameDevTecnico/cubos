#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// Plugin that allows inspecting the current settings through a ImGui window.
    ///
    /// @details This plugin adds one system, which adds a ImGui window with the current settings.
    ///
    /// Dependencies:
    /// - `imguiPlugin`
    //
    /// @param cubos CUBOS. main class
    void settingsInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
