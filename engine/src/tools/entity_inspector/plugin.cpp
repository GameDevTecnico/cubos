#include <algorithm>

#include <imgui.h>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/categorizable.hpp>
#include <cubos/core/reflection/traits/hidden.hpp>

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

// TODO: maybe only show the "Misc" category or not show it at all.
// ("Misc" is where non-categorized components fall into).
// TODO: Also show entity identifiers along with their name (See #1523).
// TODO: Make the "Select" button more clear - maybe be able to select the target entity by clicking it.

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        const Type* relationType;
        bool showHidden = false;
    };

    struct Component
    {
        CUBOS_ANONYMOUS_REFLECT(Component);

        const Type* type;
        void* value;
        size_t priority;
    };

    struct Relation
    {
        CUBOS_ANONYMOUS_REFLECT(Relation);

        const Type* type;
        void* value;
        size_t priority;
        Entity related;
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
        for (const auto& [type, name] : world.types().relations())
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

static inline void addItems(State& state, World& world, Entity& entity,
                            std::unordered_map<std::string, std::vector<Component>>& components,
                            std::unordered_map<std::string, std::vector<Relation>>& incomingRelations,
                            std::unordered_map<std::string, std::vector<Relation>>& outgoingRelations)
{
    // Add components.
    for (const auto& [type, value] : world.components(entity))
    {
        if (type->has<cubos::core::reflection::HiddenTrait>() && !state.showHidden)
        {
            continue;
        }

        std::string category = "Misc"; // Default category
        size_t priority = 0;           // Default priority

        if (type->has<cubos::core::reflection::CategorizableTrait>())
        {
            const auto& trait = type->get<cubos::core::reflection::CategorizableTrait>();
            category = trait.category();
            priority = trait.priority();
        }

        components[category].emplace_back(type, value, priority);
    }

    // Add incoming relations.
    for (const auto& [type, value, fromEntity] : world.relationsFrom(entity))
    {
        if (type->has<cubos::core::reflection::HiddenTrait>() && !state.showHidden)
        {
            continue;
        }

        std::string category = "Misc "; // One extra space so when collapsing one it does not collapse all.
        size_t priority = 0;

        if (type->has<cubos::core::reflection::CategorizableTrait>())
        {
            const auto& trait = type->get<cubos::core::reflection::CategorizableTrait>();
            category = trait.category();
            priority = trait.priority();
        }

        incomingRelations[category].emplace_back(type, value, priority, fromEntity);
    }

    // Add outgoing relations.
    for (const auto& [type, value, toEntity] : world.relationsTo(entity))
    {
        if (type->has<cubos::core::reflection::HiddenTrait>() && !state.showHidden)
        {
            continue;
        }

        std::string category = "Misc  ";
        size_t priority = 0;

        if (type->has<cubos::core::reflection::CategorizableTrait>())
        {
            const auto& trait = type->get<cubos::core::reflection::CategorizableTrait>();
            category = trait.category();
            priority = trait.priority();
        }

        outgoingRelations[category].emplace_back(type, value, priority, toEntity);
    }
}

static inline void sortItems(std::unordered_map<std::string, std::vector<Component>>& components,
                             std::unordered_map<std::string, std::vector<Relation>>& incomingRelations,
                             std::unordered_map<std::string, std::vector<Relation>>& outgoingRelations)
{
    for (auto& [category, categorized] : components)
    {
        std::ranges::sort(categorized, [](const Component& a, const Component& b) { return a.priority < b.priority; });
    }

    for (auto& [category, categorized] : incomingRelations)
    {
        std::ranges::sort(categorized, [](const Relation& a, const Relation& b) { return a.priority < b.priority; });
    }

    for (auto& [category, categorized] : outgoingRelations)
    {
        std::ranges::sort(categorized, [](const Relation& a, const Relation& b) { return a.priority < b.priority; });
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
        .call([](State& state, World& world, Toolbox& toolbox, Selection& selection, ImGuiInspector inspector,
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

                    ImGui::Checkbox("Show hidden components", &state.showHidden);

                    // Group components and relations by category.
                    std::unordered_map<std::string, std::vector<Component>> components;
                    std::unordered_map<std::string, std::vector<Relation>> incomingRelations;
                    std::unordered_map<std::string, std::vector<Relation>> outgoingRelations;

                    addItems(state, world, entity, components, incomingRelations, outgoingRelations);
                    sortItems(components, incomingRelations, outgoingRelations);

                    // Have the tree nodes open by default
                    ImGuiTreeNodeFlags treeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen;

                    const Type* removed = nullptr;

                    for (const auto& [category, categorized] : components)
                    {
                        if (ImGui::TreeNodeEx(category.c_str(), treeNodeFlag))
                        {
                            for (const auto& component : categorized)
                            {
                                ImGui::PushID(component.type->name().c_str());

                                if (ImGui::Button("X"))
                                {
                                    removed = component.type;
                                }

                                ImGui::SameLine();
                                inspector.edit(component.type->shortName(), *component.type, component.value);
                                ImGui::PopID();
                            }
                            ImGui::TreePop();
                        }
                    }

                    if (removed != nullptr)
                    {
                        world.components(entity).remove(*removed);
                        removed = nullptr;
                    }

                    ImGui::SeparatorText("Incoming Relations");
                    addRelationButton(state, world, entity, true, query);
                    Entity removedEnt{};

                    for (const auto& [category, categorized] : incomingRelations)
                    {
                        if (ImGui::TreeNodeEx(category.c_str(), treeNodeFlag))
                        {
                            for (const auto& incoming : categorized)
                            {
                                std::string relName = getName(incoming.related) + "#" + incoming.type->shortName();

                                ImGui::PushID(relName.c_str());

                                if (ImGui::Button("X"))
                                {
                                    removed = incoming.type;
                                    removedEnt = incoming.related;
                                }

                                ImGui::SameLine();
                                if (ImGui::Button("Select"))
                                {
                                    selection.entity = incoming.related;
                                }
                                ImGui::SameLine();
                                inspector.edit(relName, *incoming.type, incoming.value);
                                ImGui::PopID();
                            }
                            ImGui::TreePop();
                        }
                    }

                    if (removed != nullptr)
                    {
                        world.unrelate(entity, removedEnt, *removed);
                        removed = nullptr;
                    }

                    ImGui::SeparatorText("Outgoing Relations");
                    addRelationButton(state, world, entity, false, query);

                    for (const auto& [category, categorized] : outgoingRelations)
                    {
                        if (ImGui::TreeNodeEx(category.c_str(), treeNodeFlag))
                        {
                            for (const auto& outgoing : categorized)
                            {
                                std::string relName = outgoing.type->shortName() + "#" + getName(outgoing.related);

                                ImGui::PushID(relName.c_str());

                                if (ImGui::Button("X"))
                                {
                                    removed = outgoing.type;
                                    removedEnt = outgoing.related;
                                }

                                ImGui::SameLine();
                                if (ImGui::Button("Select"))
                                {
                                    selection.entity = outgoing.related;
                                }
                                ImGui::SameLine();
                                inspector.edit(relName, *outgoing.type, outgoing.value);
                                ImGui::PopID();
                            }
                            ImGui::TreePop();
                        }
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
