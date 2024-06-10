#include "plugin.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <imgui.h>

#include <cubos/core/ecs/name.hpp>
#include <cubos/core/memory/opt.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../entity_selector/plugin.hpp"
#include "../toolbox/plugin.hpp"

using cubos::core::ecs::Name;
using cubos::core::ecs::World;

using cubos::engine::ChildOf;
using cubos::engine::Cubos;
using cubos::engine::Entity;
using cubos::engine::Opt;
using cubos::engine::Query;

static bool isSingleWord(const std::string& query)
{
    std::istringstream stream(query);
    std::string word;
    stream >> word; // Get the first word
    return !(stream >> word);
}

static bool isValid(const std::string& query)
{
    if (query.empty())
    {
        return false;
    }
    // TODO: ADD Future query validations here
    return isSingleWord(query);
}

static void showQueries(World& world, Query<Entity, Opt<const Name&>> all, const std::string& nameQuery,
                        const std::vector<const cubos::core::reflection::Type*>& components,
                        tesseratos::EntitySelector& selector)
{
    auto textColor = (ImVec4)ImColor(255, 255, 255);

    if (components.empty() && nameQuery.empty())
    {
        return;
    }

    for (auto [entity, name] : all)
    {
        bool hasComponents = true;
        for (const auto* component : components)
        {
            if (!world.components(entity).has(*component))
            {
                hasComponents = false;
                break;
            }
        }
        if (hasComponents && name.contains() && name->value.find(nameQuery) != std::string::npos)
        {
            auto displayName = name.contains() ? name->value : std::to_string(entity.index);
            selector.selection == entity ? textColor = (ImVec4)ImColor(149, 252, 75)
                                         : textColor = (ImVec4)ImColor(255, 255, 255);

            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::Selectable(("   " + displayName).c_str(), selector.selection == entity);
            ImGui::PopStyleColor();

            if (ImGui::IsItemClicked(0))
            {
                selector.selection = entity;
            }
        }
    }
}

static void parse(World& world, const char* searchBuffer, Query<Entity, Opt<const Name&>> all,
                  tesseratos::EntitySelector& selector)
{
    std::stringstream ss(searchBuffer);
    std::string query;    // query string
    char delimiter = ','; // separator for queries
    auto worldComponents = world.types().components();
    std::vector<const cubos::core::reflection::Type*> components;
    std::string nameQuery;

    while (getline(ss, query, delimiter))
    {
        if (isValid(query)) // TODO: ADD future checks for queries here
        {
            query.erase(std::remove(query.begin(), query.end(), ' '),
                        query.end()); // remove all white-spaces from the query
            if (isupper(query[0]))    // check if the query is for a component
            {
                for (auto [componentType, string] : worldComponents)
                {
                    if (componentType->shortName() == query)
                    {
                        components.push_back(componentType);
                    }
                }
            }
            else // query is for an entity name
            {
                if (!nameQuery.empty())
                {
                    continue;
                }

                nameQuery = query;
            }
        }
    }

    showQueries(world, all, nameQuery, components, selector);
}

static void showHierarchy(Entity entity, Query<Entity, Opt<const Name&>, const ChildOf&, Entity> childOf,
                          Opt<const Name&> name, tesseratos::EntitySelector& selector)
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
            showHierarchy(child, childOf, nameChild, selector);
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
                std::string searchQuery = "";
                float window_width = ImGui::GetContentRegionAvail().x;
                static char searchBuffer[512];
                ImGui::PushItemWidth(window_width);
                ImGui::InputText("##searchQuery", searchBuffer, sizeof(searchBuffer));
                ImGui::PopItemWidth();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

                if (searchBuffer[0] != '\0')
                {
                    parse(world, searchBuffer, all, selector);
                }
                else
                {
                    for (auto [entity, name] : all)
                    {
                        if (!query.pin(0, entity).empty())
                        {
                            continue; // Skip all entities with parents
                        }

                        showHierarchy(entity, query, name, selector);
                    }
                }
            }
            ImGui::End();
        });
}
