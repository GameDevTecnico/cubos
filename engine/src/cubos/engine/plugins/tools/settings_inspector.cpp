#include <imgui.h>

#include <cubos/core/settings.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/engine/plugins/tools/settings_inspector.hpp>

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

void cubos::engine::plugins::tools::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::imguiPlugin);

    cubos.system(inspector).tagged("cubos.imgui");
}
