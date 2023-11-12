#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/imgui/serialization.hpp>
#include <cubos/engine/settings/plugin.hpp>

#include <tesseratos/settings_inspector/plugin.hpp>

using cubos::core::ecs::Write;

using cubos::engine::Cubos;
using cubos::engine::imguiEdit;
using cubos::engine::Settings;

using namespace tesseratos;

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
                imguiEdit(setting.second, setting.first);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void tesseratos::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.named("tesseratos::settingsInspectorPlugin");

    cubos.addPlugin(cubos::engine::settingsPlugin);
    cubos.addPlugin(cubos::engine::imguiPlugin);

    cubos.system(inspector).tagged("cubos.imgui");
}
