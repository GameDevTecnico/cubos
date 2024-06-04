#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/imgui/plugin.hpp>

using cubos::core::ecs::EventWriter;

using cubos::engine::Cubos;

using namespace tesseratos;

CUBOS_REFLECT_IMPL(Toolbox)
{
    return cubos::core::ecs::TypeBuilder<Toolbox>("tesseratos::Toolbox").build();
}

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
    cubos.depends(cubos::engine::imguiPlugin);

    cubos.resource<Toolbox>();

    cubos.system("show Toolbox UI").tagged(cubos::engine::imguiTag).call(showToolbox);
}
