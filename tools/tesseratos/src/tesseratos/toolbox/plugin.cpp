#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::EventWriter;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::core::ecs::Write;
using cubos::engine::Cubos;

using namespace tesseratos;

static void showToolbox(Write<Toolbox> toolbox)
{
    ImGui::Begin("Toolbox");

    for (auto& [key, val] : toolbox->toolsMap)
    {
        if (ImGui::Button(data(key)))
        {
            if (!val)
            {
                toolbox->open(key);
            }
            else
            {
                toolbox->close(key);
            }
        }
    }

    ImGui::End();
}

void tesseratos::toolboxPlugin(Cubos& cubos)
{
    cubos.addResource<Toolbox>();
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.system(showToolbox).tagged("cubos.imgui");
}
