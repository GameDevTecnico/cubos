#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// Plugin that allows inspecting the current world through a ImGui window.
    ///
    /// @details This plugin adds one system, which adds a ImGui window with the current world.
    ///
    /// Dependencies:
    /// - `imguiPlugin`
    //
    /// @param cubos CUBOS. main class
    void worldInspectorPlugin(Cubos& cubos);
}; // namespace cubos::engine::tools