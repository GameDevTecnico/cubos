#include <imgui.h>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/entity_inspector/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

using cubos::engine::Cubos;
using cubos::engine::DataInspector;

void tesseratos::entityInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.system("show Entity Inspector UI").tagged("cubos.imgui").call([](World& world) {
        if (!world.write<Toolbox>().get().isOpen("Entity Inspector"))
        {
            return;
        }

        ImGui::Begin("Entity Inspector");
        if (!ImGui::IsWindowCollapsed())
        {
            auto selection = world.read<EntitySelector>().get().selection;

            if (!selection.isNull() && world.isAlive(selection))
            {
                ImGui::Text("Entity %d selected", selection.index);
                ImGui::BeginTable("showEntity", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
                auto dataInspector = world.write<DataInspector>();

                ImGui::TableNextRow();

                for (auto [type, value] : world.components(selection))
                {
                    if (dataInspector.get().edit(type->name(), *type, value))
                    {
                        // ...
                    }
                }
                ImGui::EndTable();
            }
            else
            {
                ImGui::Text("No entity selected");
            }
        }
        ImGui::End();
    });
}
