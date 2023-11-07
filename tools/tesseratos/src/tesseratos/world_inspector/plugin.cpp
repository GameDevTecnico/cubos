#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>
#include <tesseratos/world_inspector/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;

using cubos::engine::Cubos;

using namespace tesseratos;

static void inspectWorld(Write<World> world)
{
    if (!(world->write<Toolbox>().get().isOpen("World Inspector")))
    {
        return;
    }

    auto selector = world->write<EntitySelector>();

    ImGui::Begin("World Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        int n = 0;
        for (auto entity : *world)
        {
            ImGui::PushID(n);

            ImGui::BulletText("%s", std::to_string(entity.index).c_str());

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

void tesseratos::worldInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(toolboxPlugin);
    cubos.addPlugin(entitySelectorPlugin);
    cubos.system(inspectWorld).tagged("cubos.imgui");
}
