#include <cubos/engine/tools/world_inspector/plugin.hpp>
#include <imgui.h>

#include <cubos/engine/plugins/imgui.hpp>
#include <cubos/engine/plugins/tools/entity_selector.hpp>

using cubos::core::ecs::World;
using cubos::engine::plugins::tools::EntitySelector;

static void inspectWorld(World& world, EntitySelector& selector)
{
    ImGui::Begin("World Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        int n = 0;
        for (auto entity : world)
        {
            ImGui::PushID(n);

            ImGui::BulletText(std::to_string(n).c_str());

            ImGui::SameLine();
            if (ImGui::Button("Select"))
            {
                selector.selection = entity;
            }

            n++;
            ImGui::PopID();
        }
    }
    ImGui::End();
}

void cubos::engine::tools::worldInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::imguiPlugin);
    cubos.addPlugin(cubos::engine::plugins::tools::entitySelectorPlugin);

    cubos.system(inspectWorld).tagged("cubos.imgui");
}