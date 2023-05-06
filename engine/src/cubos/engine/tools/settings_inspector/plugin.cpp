#include <imgui.h>

#include <cubos/core/settings.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>

using namespace cubos::engine;

static void inspector(cubos::core::Settings& settings)
{
    ImGui::Begin("Settings Inspector");

    const auto& map = settings.getValues();
    if (map.empty())
    {
        ImGui::Text("No settings found.");
    }
    else
    {
        ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
        for (const auto& setting : map)
        {
            cubos::core::ui::show(setting.second, setting.first);
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

void cubos::engine::tools::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);

    cubos.system(inspector).tagged("cubos.imgui");
}
