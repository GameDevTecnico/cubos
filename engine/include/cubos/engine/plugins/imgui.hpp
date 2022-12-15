#ifndef CUBOS_ENGINE_PLUGINS_IMGUI_HPP
#define CUBOS_ENGINE_PLUGINS_IMGUI_HPP

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin that adds ImGui functionalities and allows ImGui windows to be rendered.
    /// @param cubos CUBOS. main class
    void imguiPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_IMGUI_HPP
