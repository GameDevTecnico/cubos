#include <cubos/engine/plugins/tools/settings_inspector.hpp>
#include <cubos/core/settings.hpp>
#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>
#include <imgui.h>

static void settingsInspectorSystem(cubos::core::Settings& settings)
{
    ImGui::Begin("Settings Inspector");

    bool noSettings = settings.begin() == settings.end();

    if (noSettings)
    {
        ImGui::Text("No settings found.");
    }
    else
    {
        ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
        for (const auto& s : settings)
        {
            cubos::core::ui::show(s.second, s.first);
            noSettings = false;
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

void cubos::engine::plugins::tools::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::imguiPlugin)
        .addSystem(settingsInspectorSystem, "SettingsInspector")
        .putStageAfter("SettingsInspector", "BeginImGuiFrame");
}
