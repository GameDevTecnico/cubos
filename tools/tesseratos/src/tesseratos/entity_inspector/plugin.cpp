#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/imgui/serialization.hpp>

#include <tesseratos/entity_inspector/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>

using cubos::core::data::old::Context;
using cubos::core::data::old::SerializationMap;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;

using cubos::engine::Cubos;
using cubos::engine::imguiEditPackage;

using namespace tesseratos;

static void inspectEntity(Write<World> world)
{
    auto selection = world->read<EntitySelector>().get().selection;

    ImGui::Begin("Entity Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        ImGui::BeginTable("showEntity", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

        if (!selection.isNull() && world->isAlive(selection))
        {
            auto pkg = world->pack(selection);
            if (imguiEditPackage(pkg, std::to_string(selection.index)))
            {
                world->unpack(selection, pkg);
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void tesseratos::entityInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(entitySelectorPlugin);

    cubos.system(inspectEntity).tagged("cubos.imgui");
}
