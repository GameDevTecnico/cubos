#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>
#include <cubos/engine/tools/utils/serialization.hpp>

using cubos::core::ecs::Write;

using namespace cubos::engine;

static void inspector(Write<Settings> settings)
{
    ImGui::Begin("Settings Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        auto& map = settings->getValues();
        if (map.empty())
        {
            ImGui::Text("No settings found.");
        }
        else
        {
            ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
            for (auto& setting : map)
            {
                tools::edit(setting.second, setting.first);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void cubos::engine::tools::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(settingsPlugin);
    cubos.addPlugin(imguiPlugin);

    cubos.system(inspector).tagged("cubos.imgui");
}
