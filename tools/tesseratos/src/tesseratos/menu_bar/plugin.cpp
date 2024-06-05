#include "plugin.hpp"
#include <algorithm>

#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Traversal;
using namespace cubos::engine;
using namespace tesseratos;

namespace
{
    struct Item
    {
        Entity entity;
        MenuBarItem item;
        std::vector<Item> items;
    };
} // namespace

static void getChildItems(Item& parent, Query<Entity, const MenuBarItem&> childOf)
{
    for (auto [entity, item] : childOf.pin(1, parent.entity))
    {
        Item child{entity, item, {}};
        getChildItems(child, childOf);
        parent.items.emplace_back(child);
    }

    std::sort(parent.items.begin(), parent.items.end(), [](const Item& a, const Item& b) {
        return a.item.order < b.item.order || (a.item.order == b.item.order && a.item.name < b.item.name);
    });
}

static void showItem(Commands& cmds, const Item& item)
{
    if (item.items.empty())
    {
        if (ImGui::MenuItem(item.item.name.c_str()))
        {
            // Trigger any observers of the selected item.
            cmds.add(item.entity, MenuBarSelected{});
            cmds.remove<MenuBarSelected>(item.entity);
        }
    }
    else
    {
        if (ImGui::BeginMenu(item.item.name.c_str()))
        {
            for (auto& child : item.items)
            {
                showItem(cmds, child);
            }
            ImGui::EndMenu();
        }
    }
}

void tesseratos::menuBarPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(transformPlugin);

    cubos.component<MenuBarItem>();
    cubos.component<MenuBarSelected>();

    cubos.system("draw MenuBar")
        .tagged(imguiTag)
        .with<MenuBarItem>()
        .related<ChildOf>()
        .call([](Commands cmds, Query<Entity, const MenuBarItem&> childOf, Query<Entity, const MenuBarItem&> items) {
            if (!ImGui::BeginMainMenuBar())
            {
                return;
            }

            // Extract item tree from the queries, sorted by the order and name item fields.
            std::vector<Item> processed{};
            for (auto [entity, item] : items)
            {
                if (childOf.pin(0, entity).empty())
                {
                    Item root{entity, item, {}};
                    getChildItems(root, childOf);
                    processed.push_back(root);
                }
            }
            std::sort(processed.begin(), processed.end(), [](const Item& a, const Item& b) {
                return a.item.order < b.item.order || (a.item.order == b.item.order && a.item.name < b.item.name);
            });

            // Show the menu bar.
            for (const auto& item : processed)
            {
                showItem(cmds, item);
            }

            ImGui::EndMainMenuBar();
        });
}
