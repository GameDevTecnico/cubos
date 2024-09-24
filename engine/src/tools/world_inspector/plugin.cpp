#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <imgui.h>

#include <cubos/core/ecs/name.hpp>
#include <cubos/core/memory/opt.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>
#include <cubos/engine/tools/world_inspector/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Name;
using cubos::core::ecs::World;

using namespace cubos::engine;

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
                        const std::vector<const cubos::core::reflection::Type*>& components, Selection& selection)
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
            selection.entity == entity ? textColor = (ImVec4)ImColor(149, 252, 75)
                                       : textColor = (ImVec4)ImColor(255, 255, 255);

            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::Selectable(("   " + displayName).c_str(), selection.entity == entity);
            ImGui::PopStyleColor();

            if (ImGui::IsItemClicked(0))
            {
                selection.entity = entity;
            }
        }
    }
}

static void parse(World& world, const char* searchBuffer, Query<Entity, Opt<const Name&>> all, Selection& selection)
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
                        query.end());   // remove all white-spaces from the query
            if (isupper(query[0]) != 0) // check if the query is for a component
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

    showQueries(world, all, nameQuery, components, selection);
}

static void showHierarchy(Entity entity, Query<Entity, Opt<const Name&>, const ChildOf&, Entity> childOf,
                          Opt<const Name&> name, Selection& selection)
{
    auto displayName = name.contains() ? name->value : std::to_string(entity.index);
    auto children = childOf.pin(1, entity);
    auto textColor = (ImVec4)ImColor(255, 255, 255);

    selection.entity == entity ? textColor = (ImVec4)ImColor(149, 252, 75) : textColor = (ImVec4)ImColor(255, 255, 255);

    // check if entity has children
    if (children.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::Selectable(("   " + displayName).c_str(), selection.entity == entity);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked(0))
        {
            selection.entity = entity;
        }

        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    if (ImGui::TreeNode(displayName.c_str()))
    {
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked(0))
        {
            selection.entity = entity;
        }
        for (auto [child, nameChild, _, parent] : children)
        {
            showHierarchy(child, childOf, nameChild, selection);
        }
        ImGui::TreePop();

        return;
    }
    ImGui::PopStyleColor();
}

void cubos::engine::worldInspectorPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(toolboxPlugin);
    cubos.depends(selectionPlugin);

    cubos.system("show World Inspector UI")
        .tagged(imguiTag)
        .call([](World& world, Toolbox& toolbox, Selection& selection, Query<Entity, Opt<const Name&>> all,
                 Query<Entity, Opt<const Name&>, const ChildOf&, Entity> query) {
            if (!(toolbox.isOpen("World Inspector")))
            {
                return;
            }

            ImGui::Begin("World Inspector");

            if (!ImGui::IsWindowCollapsed())
            {
                std::string searchQuery{};
                float windowWidth = ImGui::GetContentRegionAvail().x;
                static char searchBuffer[512];
                ImGui::PushItemWidth(windowWidth);
                ImGui::InputText("##searchQuery", searchBuffer, sizeof(searchBuffer));
                ImGui::PopItemWidth();
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Spacing();

                if (searchBuffer[0] != '\0')
                {
                    parse(world, searchBuffer, all, selection);
                }
                else
                {
                    for (auto [entity, name] : all)
                    {
                        if (!query.pin(0, entity).empty())
                        {
                            continue; // Skip all entities with parents
                        }

                        showHierarchy(entity, query, name, selection);
                    }
                }
            }
            ImGui::End();
        });
}
