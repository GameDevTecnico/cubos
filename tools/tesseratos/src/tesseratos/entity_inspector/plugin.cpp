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
using cubos::core::memory::AnyValue;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

using cubos::engine::Cubos;
using cubos::engine::DataInspector;

void tesseratos::entityInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.system("show Entity Inspector UI")
        .tagged(cubos::engine::imguiTag)
        .call([](World& world, Toolbox& toolbox, const EntitySelector& entitySelector, DataInspector& dataInspector) {
            if (!toolbox.isOpen("Entity Inspector"))
            {
                return;
            }

            ImGui::Begin("Entity Inspector");
            if (!ImGui::IsWindowCollapsed())
            {
                auto entity = entitySelector.selection;
                if (!entity.isNull() && world.isAlive(entity))
                {
                    ImGui::Text("Entity %d#%d selected", entity.index, entity.generation);
                    ImGui::Separator();

                    if (ImGui::Button("Add Component"))
                    {
                        ImGui::OpenPopup("Select Component Type");
                    }

                    if (ImGui::BeginPopup("Select Component Type"))
                    {
                        for (auto [type, name] : world.types().components())
                        {
                            if (ImGui::Button(name.c_str()))
                            {
                                auto value = AnyValue::defaultConstruct(*type);
                                world.components(entity).add(value.type(), value.get());
                                ImGui::CloseCurrentPopup();
                            }
                        }

                        ImGui::EndPopup();
                    }

                    const Type* removed = nullptr;
                    for (auto [type, value] : world.components(entity))
                    {
                        ImGui::SeparatorText(type->name().c_str());
                        dataInspector.edit(*type, value);
                        if (ImGui::Button("Remove Component"))
                        {
                            removed = type;
                        }
                    }

                    if (removed != nullptr)
                    {
                        world.components(entity).remove(*removed);
                    }
                }
                else
                {
                    ImGui::Text("No entity selected");
                }
            }
            ImGui::End();
        });
}
