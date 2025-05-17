#include <imgui.h>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/entity_inspector/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

using cubos::core::ecs::Name;
using cubos::core::ecs::World;
using cubos::core::memory::AnyValue;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

using cubos::engine::Entity;
using cubos::engine::Opt;
using cubos::engine::Query;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        const Type* relationType;
    };
} // namespace

static void addRelationButton(State& state, World& world, Entity entity, bool incoming,
                              Query<Entity, Opt<const Name&>>& query)
{
    std::string suffix = incoming ? "##incoming" : "##outgoing";

    if (ImGui::Button(("Add Relation" + suffix).c_str()))
    {
        ImGui::OpenPopup(("Select Relation Type" + suffix).c_str());
    }

    bool openPopup = false;
    if (ImGui::BeginPopup(("Select Relation Type" + suffix).c_str()))
    {
        for (auto [type, name] : world.types().relations())
        {
            if (ImGui::Button(name.c_str()))
            {
                state.relationType = type;
                openPopup = true;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }

    if (openPopup)
    {
        ImGui::OpenPopup(("Select Relation Target" + suffix).c_str());
    }

    if (ImGui::BeginPopup(("Select Relation Target" + suffix).c_str()))
    {
        for (auto [target, name] : query)
        {
            std::string targetStr =
                name ? name->value : (std::to_string(target.index) + "#" + std::to_string(target.generation));
            if (ImGui::Button(targetStr.c_str()))
            {
                auto value = AnyValue::defaultConstruct(*state.relationType);
                if (incoming)
                {
                    world.relate(target, entity, value.type(), value.get());
                }
                else
                {
                    world.relate(entity, target, value.type(), value.get());
                }
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void cubos::engine::entityInspectorPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(selectionPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.system("show Entity Inspector UI")
        .tagged(imguiTag)
        .call([](State& state, World& world, Toolbox& toolbox, const Selection& selection, ImGuiInspector inspector,
                 Query<Entity, Opt<const Name&>> query) {
            if (!toolbox.isOpen("Entity Inspector"))
            {
                return;
            }

            auto getName = [&](Entity ent) -> std::string {
                if (!world.isAlive(ent))
                {
                    return "Invalid";
                }

                if (world.components(ent).has<Name>())
                {
                    return world.components(ent).get<Name>().value;
                }

                return std::to_string(ent.index) + "#" + std::to_string(ent.generation);
            };

            ImGui::Begin("Entity Inspector");
            if (!ImGui::IsWindowCollapsed())
            {
                auto entity = selection.entity;
                if (!entity.isNull() && world.isAlive(entity))
                {
                    ImGui::Text("Entity %s selected", getName(entity).c_str());
                    ImGui::SeparatorText("Components");

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

                    //TODO: group components into category
                    // category is in a reflection trait called categorize (TODO)
                    const Type* removed = nullptr;
                    for (auto [type, value] : world.components(entity))
                    {
                        ImGui::PushID(type->name().c_str());
                        if (ImGui::Button("X"))
                        {
                            removed = type;
                        }
                        ImGui::SameLine();
                        inspector.edit(type->shortName(), *type, value);
                        ImGui::PopID();
                    }

                    if (removed != nullptr)
                    {
                        world.components(entity).remove(*removed);
                        removed = nullptr;
                    }

                    ImGui::SeparatorText("Incoming Relations");
                    addRelationButton(state, world, entity, true, query);

                    Entity removedEnt{};
                    for (auto [type, value, fromEntity] : world.relationsTo(entity))
                    {
                        std::string relName = getName(fromEntity) + "#" + type->shortName();
                        ImGui::PushID(relName.c_str());
                        if (ImGui::Button("X"))
                        {
                            removed = type;
                            removedEnt = fromEntity;
                        }
                        ImGui::SameLine();
                        inspector.edit(relName, *type, value);
                        ImGui::PopID();
                    }

                    if (removed != nullptr)
                    {
                        world.unrelate(removedEnt, entity, *removed);
                        removed = nullptr;
                    }

                    ImGui::SeparatorText("Outgoing Relations");
                    addRelationButton(state, world, entity, false, query);

                    for (auto [type, value, toEntity] : world.relationsFrom(entity))
                    {
                        std::string relName = type->shortName() + "#" + getName(toEntity);
                        ImGui::PushID(relName.c_str());
                        if (ImGui::Button("X"))
                        {
                            removed = type;
                            removedEnt = toEntity;
                        }
                        ImGui::SameLine();
                        inspector.edit(relName, *type, value);
                        ImGui::PopID();
                    }

                    if (removed != nullptr)
                    {
                        world.unrelate(entity, removedEnt, *removed);
                        removed = nullptr;
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
