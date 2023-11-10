#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::EventWriter;
using cubos::core::ecs::Read;
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

auto Toolbox::begin()
{
    return mToolsMap.begin();
}

auto Toolbox::end()
{
    return mToolsMap.end();
}

static void showToolbox(Write<Toolbox> toolbox)
{
    ImGui::Begin("Toolbox");

    for (auto it = toolbox->begin(); it != toolbox->end(); ++it)
    {
        if (ImGui::Button(data(it->first)))
        {
            toolbox->doClick(it->first);
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
