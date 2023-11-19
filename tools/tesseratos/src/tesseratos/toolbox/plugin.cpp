#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::EventWriter;
using cubos::core::ecs::Write;

using cubos::core::ecs::Write;
using cubos::engine::Cubos;

using namespace tesseratos;

bool Toolbox::isOpen(std::string toolName)
{
    if (mToolsMap.contains(toolName))
    {
        return mToolsMap[toolName];
    }
    mToolsMap[toolName] = false;
    return false;
}

void Toolbox::open(std::string toolName)
{
    mToolsMap[toolName] = true;
}

void Toolbox::close(std::string toolName)
{
    mToolsMap[toolName] = false;
}

void Toolbox::doClick(std::string toolName)
{
    mToolsMap[toolName] = !mToolsMap[toolName];
}

static void showToolbox(Write<Toolbox> toolbox)
{
    ImGui::Begin("Toolbox");

    for (const auto& [tool, open] : *toolbox)
    {
        bool check = open;
        if (ImGui::Checkbox(data(tool), &check))
        {
            toolbox->doClick(tool);
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
