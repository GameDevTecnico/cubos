#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/entity_selector/plugin.hpp>
#include <cubos/engine/tools/world_inspector/plugin.hpp>

using cubos::core::ecs::World;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static void inspectWorld(Write<World> world)
{
    auto selector = world->write<tools::EntitySelector>();

    ImGui::Begin("World Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        int n = 0;
        for (auto entity : *world)
        {
            ImGui::PushID(n);

            ImGui::BulletText("%s", std::to_string(n).c_str());

            ImGui::SameLine();
            if (ImGui::Button("Select"))
            {
                selector.get().selection = entity;
            }

            n++;
            ImGui::PopID();
        }
    }
    ImGui::End();
}

void cubos::engine::tools::worldInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(imguiPlugin);
    cubos.addPlugin(tools::entitySelectorPlugin);

    cubos.system(inspectWorld).tagged("cubos.imgui");
}