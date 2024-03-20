#include <imgui.h>

#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/query/opt.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>
#include <tesseratos/world_inspector/plugin.hpp>

using cubos::core::ecs::Name;
using cubos::core::ecs::World;

using cubos::engine::ChildOf;
using cubos::engine::Cubos;
using cubos::engine::Entity;
using cubos::engine::Opt;
using cubos::engine::Query;

static void show(Entity entity, Query<Entity, Opt<const Name&>, const ChildOf&, Entity> childOf, Opt<const Name&> name,
                 tesseratos::EntitySelector& selector)
{
    auto displayName = name.contains() ? name->value : std::to_string(entity.index);
    auto children = childOf.pin(1, entity);
    auto textColor = (ImVec4)ImColor(255, 255, 255);

    selector.selection == entity ? textColor = (ImVec4)ImColor(149, 252, 75)
                                 : textColor = (ImVec4)ImColor(255, 255, 255);

    // check if entity has children
    if (children.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::Selectable(("   " + displayName).c_str(), selector.selection == entity);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked(0))
        {
            selector.selection = entity;
        }

        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    if (ImGui::TreeNode(displayName.c_str()))
    {
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked(0))
        {
            selector.selection = entity;
        }
        for (auto [child, nameChild, _, parent] : children)
        {
            show(child, childOf, nameChild, selector);
        }
        ImGui::TreePop();

        return;
    }
    ImGui::PopStyleColor();
}

void tesseratos::worldInspectorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::transformPlugin);
    cubos.depends(toolboxPlugin);
    cubos.depends(entitySelectorPlugin);

    cubos.system("show World Inspector UI")
        .tagged(cubos::engine::imguiTag)
        .call([](World& world, Query<Entity, Opt<const Name&>> all,
                 Query<Entity, Opt<const Name&>, const ChildOf&, Entity> query) {
            if (!(world.resource<Toolbox>().isOpen("World Inspector")))
            {
                return;
            }

            auto& selector = world.resource<EntitySelector>();

            ImGui::Begin("World Inspector");
            if (!ImGui::IsWindowCollapsed())
            {
                for (auto [entity, name] : all)
                {
                    if (!query.pin(0, entity).empty())
                    {
                        continue; // Skip all entities with parents
                    }

                    show(entity, query, name, selector);
                }
            }
            ImGui::End();
        });
}
