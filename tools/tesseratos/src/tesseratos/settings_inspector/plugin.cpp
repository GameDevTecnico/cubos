#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

#include "../toolbox/plugin.hpp"

using cubos::core::reflection::reflect;

using cubos::engine::Cubos;
using cubos::engine::DataInspector;
using cubos::engine::Settings;

void tesseratos::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::settingsPlugin);
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.system("show Settings Inspector UI")
        .tagged(cubos::engine::imguiTag)
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
                    inspector.edit(map);
                }
            }
            ImGui::End();
        });
}
