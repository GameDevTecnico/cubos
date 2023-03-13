#ifndef CUBOS_ENGINE_PLUGINS_SETTINGS_INSPECTOR_HPP
#define CUBOS_ENGINE_PLUGINS_SETTINGS_INSPECTOR_HPP

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins::tools
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
} // namespace cubos::engine::plugins::tools

#endif // CUBOS_ENGINE_PLUGINS_SETTINGS_INSPECTOR_HPP
