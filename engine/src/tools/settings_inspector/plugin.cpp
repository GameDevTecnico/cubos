#include <imgui.h>

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

void cubos::engine::settingsInspectorPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.system("show Settings Inspector UI")
        .tagged(imguiTag)
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
