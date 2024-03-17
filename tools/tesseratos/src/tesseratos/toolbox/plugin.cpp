#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::EventWriter;

using cubos::engine::Cubos;

using namespace tesseratos;

bool Toolbox::isOpen(const std::string& toolName)
{
    if (mToolsMap.contains(toolName))
    {
        return mToolsMap[toolName];
    }
    mToolsMap[toolName] = false;
    return false;
}

void Toolbox::open(const std::string& toolName)
{
    mToolsMap[toolName] = true;
}

void Toolbox::close(const std::string& toolName)
{
    mToolsMap[toolName] = false;
}

void Toolbox::toggle(const std::string& toolName)
{
    mToolsMap[toolName] = !mToolsMap[toolName];
}

static void showToolbox(Toolbox& toolbox)
{
    ImGui::Begin("Toolbox");

    for (const auto& [tool, open] : toolbox)
    {
        bool check = open;
        if (ImGui::Checkbox(data(tool), &check))
        {
            toolbox.toggle(tool);
        }
    }

    ImGui::End();
}

void tesseratos::toolboxPlugin(Cubos& cubos)
{
    cubos.addResource<Toolbox>();
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.system("show Toolbox UI").tagged(cubos::engine::ImguiTag).call(showToolbox);
}
