#include <imgui.h>

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

#include <tesseratos/settings_inspector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::reflection::reflect;

using cubos::engine::Cubos;
using cubos::engine::DataInspector;
using cubos::engine::Settings;

void tesseratos::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::settingsPlugin);
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.system("show Settings Inspector UI")
        .tagged("cubos.imgui")
        .call([](Settings& settings, DataInspector& inspector, Toolbox& toolbox) {
            if (!toolbox.isOpen("Settings Inspector"))
            {
                return;
            }

            ImGui::Begin("Settings Inspector");
            if (!ImGui::IsWindowCollapsed())
            {
                auto& map = settings.getValues();
                if (map.empty())
                {
                    ImGui::Text("No settings found.");
                }
                else
                {
                    ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
                    inspector.edit("Settings", map);
                    ImGui::EndTable();
                }
            }
            ImGui::End();
        });
}
