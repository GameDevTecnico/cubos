#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

void cubos::engine::toolboxPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);

    cubos.resource<Toolbox>();

    cubos.system("show Toolbox UI").tagged(imguiTag).call([](Toolbox& toolbox) {
        if (!ImGui::Begin("Toolbox"))
        {
            ImGui::End();
            return;
        }

        for (const auto& [tool, open] : toolbox)
        {
            bool check = open;
            if (ImGui::Checkbox(data(tool), &check))
            {
                toolbox.toggle(tool);
            }
        }

        ImGui::End();
    });
}
