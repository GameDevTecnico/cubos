#include <imgui.h>

#include <cubos/core/ecs/name.hpp>

#include <cubos/engine/imgui/inspector.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/ecs_statistics/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

using namespace cubos::core::ecs;
using cubos::core::reflection::Type;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        char columnFilter[256]{0};
        ArchetypeId selectedArchetype{ArchetypeId::Invalid};
        DataTypeId selectedSparseRelationTypeId{DataTypeId::Invalid};
        SparseRelationTableId selectedSparseRelationTableId{};
        bool showInactiveArchetypes{false};
        bool showInactiveSparseRelationTables{false};
        const Type* selectedResourceType{nullptr};
    };
} // namespace

void cubos::engine::ecsStatisticsPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(toolboxPlugin);
    cubos.depends(selectionPlugin);

    cubos.resource<State>();

    cubos.system("show ECS statistics")
        .tagged(imguiTag)
        .onlyIf([](Toolbox& toolbox) { return toolbox.isOpen("ECS Statistics"); })
        .call([](const World& world, State& state, Selection& selection, ImGuiInspector inspector) {
            if (world.isAlive(selection.entity))
            {
                state.selectedArchetype = world.archetype(selection.entity);
            }

            ImGui::Begin("ECS Statistics");

            ImGui::BeginTabBar("ECS Statistics Tabs");

            if (ImGui::BeginTabItem("Archetypes"))
            {

                ImGui::Text("Entity Count: %d", static_cast<int>(world.entityCount()));

                // Get a vector of all archetypes, and filter out the empty archetypes.
                std::vector<ArchetypeId> allArchetypes;
                world.archetypeGraph().collect(ArchetypeId::Empty, allArchetypes);
                std::vector<ArchetypeId> activeArchetypes;
                for (const auto& archetype : allArchetypes)
                {
                    if (world.tables().dense().contains(archetype) && world.tables().dense().at(archetype).size() > 0)
                    {
                        activeArchetypes.push_back(archetype);
                    }
                }
                ImGui::Text("Archetype Count: %d (%d inactive)", static_cast<int>(activeArchetypes.size()),
                            static_cast<int>(allArchetypes.size() - activeArchetypes.size()));

                ImGui::Separator();

                ImGui::Checkbox("Show Inactive", &state.showInactiveArchetypes);
                ImGui::InputText("Filter by Column", state.columnFilter, sizeof(state.columnFilter),
                                 ImGuiInputTextFlags_AutoSelectAll);
                if (ImGui::BeginTable("Archetypes", 3,
                                      ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                                          ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollX))
                {
                    ImGui::TableSetupColumn("ID");
                    ImGui::TableSetupColumn("Size");
                    ImGui::TableSetupColumn("Columns");
                    ImGui::TableHeadersRow();
                    int id = 0;
                    for (const auto& archetype : state.showInactiveArchetypes ? allArchetypes : activeArchetypes)
                    {
                        std::string columns{};
                        for (auto column = world.archetypeGraph().first(archetype); column != ColumnId::Invalid;
                             column = world.archetypeGraph().next(archetype, column))
                        {
                            if (!columns.empty())
                            {
                                columns += ", ";
                            }
                            columns += world.types().type(column.dataType()).name();
                        }

                        if (columns.find(state.columnFilter) == std::string::npos)
                        {
                            continue;
                        }

                        ImGui::PushID(id++);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Button(std::to_string(archetype.inner).c_str()))
                        {
                            state.selectedArchetype = archetype;
                            selection.entity = {};
                        }
                        ImGui::TableNextColumn();
                        if (world.tables().dense().contains(archetype))
                        {
                            ImGui::Text("%d", static_cast<int>(world.tables().dense().at(archetype).size()));
                        }
                        else
                        {
                            ImGui::Text("0");
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", columns.c_str());
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }

                ImGui::PushID("Selected Archetype");
                bool continueSelectedArchetype = true;
                if (state.selectedArchetype != ArchetypeId::Invalid &&
                    ImGui::CollapsingHeader("Selected Archetype", &continueSelectedArchetype,
                                            ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("Id: %d", static_cast<int>(state.selectedArchetype.inner));
                    if (!world.tables().dense().contains(state.selectedArchetype))
                    {
                        ImGui::Text("Entity Count: 0");
                    }
                    else
                    {
                        const auto& table = world.tables().dense().at(state.selectedArchetype);
                        ImGui::Text("Entity Count: %d", static_cast<int>(table.size()));

                        if (ImGui::TreeNode("Columns"))
                        {
                            for (auto column = world.archetypeGraph().first(state.selectedArchetype);
                                 column != ColumnId::Invalid;
                                 column = world.archetypeGraph().next(state.selectedArchetype, column))
                            {
                                auto name = world.types().type(column.dataType()).name();
                                ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf);
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }

                        if (ImGui::BeginTable("Selected Archetype", 2,
                                              ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                                                  ImGuiTableFlags_BordersOuter))
                        {
                            ImGui::TableSetupColumn("Row");
                            ImGui::TableSetupColumn("Entity", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableHeadersRow();
                            for (std::size_t row = 0; row < table.size(); ++row)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text("%d", static_cast<int>(row));
                                ImGui::TableNextColumn();
                                Entity entity{table.entity(row), world.generation(table.entity(row))};
                                auto name = std::to_string(entity.index) + "#" + std::to_string(entity.generation);
                                if (world.components(entity).has<Name>())
                                {
                                    name = world.components(entity).get<Name>().value;
                                }
                                if (ImGui::Button(name.c_str()))
                                {
                                    selection.entity = entity;
                                }
                            }
                            ImGui::EndTable();
                        }
                    }

                    if (!continueSelectedArchetype)
                    {
                        state.selectedArchetype = ArchetypeId::Invalid;
                    }
                }
                ImGui::PopID();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Sparse Relations"))
            {
                const auto& registry = world.tables().sparseRelation();

                if (ImGui::BeginTable("Sparse Relation Types", 3,
                                      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable |
                                          ImGuiTableFlags_SizingFixedFit))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Active Tables");
                    ImGui::TableSetupColumn("Inactive Tables");
                    ImGui::TableHeadersRow();
                    for (const auto& [dataTypeId, index] : registry)
                    {
                        int active = 0;
                        int inactive = 0;

                        for (const auto& [archetype, tables] : index.from())
                        {
                            for (const auto& table : tables)
                            {
                                if (registry.at(table).size() > 0)
                                {
                                    ++active;
                                }
                                else
                                {
                                    ++inactive;
                                }
                            }
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Button(world.types().type(dataTypeId).name().c_str()))
                        {
                            state.selectedSparseRelationTypeId = dataTypeId;
                            state.selectedSparseRelationTableId = {};
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", active);
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", inactive);
                    }
                    ImGui::EndTable();
                }

                ImGui::PushID("Selected Sparse Relation Type");
                bool continueSelectedSparseRelationType = true;
                if (state.selectedSparseRelationTypeId != DataTypeId::Invalid &&
                    ImGui::CollapsingHeader("Selected Sparse Relation Type", &continueSelectedSparseRelationType,
                                            ImGuiTreeNodeFlags_DefaultOpen))
                {
                    SparseRelationTableRegistry::CacheCursor cacheCursor{};
                    std::vector<SparseRelationTableId> tables{};
                    int activeTableCount = 0;
                    world.tables().sparseRelation().forEach(
                        cacheCursor, [&](bool /*cleanup*/, SparseRelationTableId tableId) {
                            if (tableId.dataType == state.selectedSparseRelationTypeId)
                            {
                                tables.push_back(tableId);

                                if (world.tables().sparseRelation().at(tableId).size() > 0)
                                {
                                    ++activeTableCount;
                                }
                            }
                        });

                    ImGui::Separator();
                    ImGui::Text("Selected Relation Type: %s",
                                world.types().type(state.selectedSparseRelationTypeId).name().c_str());
                    ImGui::Text("Is Symmetric: %s",
                                world.types().isSymmetricRelation(state.selectedSparseRelationTypeId) ? "true"
                                                                                                      : "false");
                    ImGui::Text("Is Tree: %s",
                                world.types().isTreeRelation(state.selectedSparseRelationTypeId) ? "true" : "false");
                    ImGui::Text("Active Table Count: %d", activeTableCount);
                    ImGui::Checkbox("Show Inactive", &state.showInactiveSparseRelationTables);

                    if (ImGui::BeginTable("Selected Sparse Relation Type Tables", 5,
                                          ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                                              ImGuiTableFlags_BordersOuter))
                    {
                        ImGui::TableSetupColumn("");
                        ImGui::TableSetupColumn("From Archetype");
                        ImGui::TableSetupColumn("To Archetype");
                        ImGui::TableSetupColumn("Depth");
                        ImGui::TableSetupColumn("Size");
                        ImGui::TableHeadersRow();
                        int id = 0;
                        for (const auto& tableId : tables)
                        {
                            if (!state.showInactiveSparseRelationTables &&
                                world.tables().sparseRelation().at(tableId).size() == 0)
                            {
                                continue;
                            }

                            ImGui::PushID(id++);
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if (ImGui::Button("Select"))
                            {
                                state.selectedSparseRelationTableId = tableId;
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", static_cast<int>(tableId.from.inner));
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", static_cast<int>(tableId.to.inner));
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", static_cast<int>(tableId.depth));
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", static_cast<int>(world.tables().sparseRelation().at(tableId).size()));
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }

                    if (!continueSelectedSparseRelationType)
                    {
                        state.selectedSparseRelationTypeId = DataTypeId::Invalid;
                        state.selectedSparseRelationTableId = {};
                    }
                }
                ImGui::PopID();

                ImGui::PushID("Selected Sparse Relation Table");
                bool continueSelectedSparseRelationTable = true;
                if (state.selectedSparseRelationTableId.dataType != DataTypeId::Invalid &&
                    ImGui::CollapsingHeader("Selected Sparse Relation Table", &continueSelectedSparseRelationTable,
                                            ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto& table = world.tables().sparseRelation().at(state.selectedSparseRelationTableId);

                    ImGui::Separator();
                    ImGui::Text("Relation Type: %s",
                                world.types().type(state.selectedSparseRelationTableId.dataType).name().c_str());
                    ImGui::Text("From Archetype: %d", static_cast<int>(state.selectedSparseRelationTableId.from.inner));
                    ImGui::Text("To Archetype: %d", static_cast<int>(state.selectedSparseRelationTableId.to.inner));
                    ImGui::Text("Depth: %d", state.selectedSparseRelationTableId.depth);
                    ImGui::Text("Size: %d", static_cast<int>(table.size()));

                    if (ImGui::BeginTable("Selected Sparse Relation Table", 3,
                                          ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                                              ImGuiTableFlags_BordersOuter))
                    {
                        ImGui::TableSetupColumn("Row");
                        ImGui::TableSetupColumn("From Entity", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("To Entity", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();
                        for (std::size_t row = 0; row < table.size(); ++row)
                        {
                            ImGui::PushID(static_cast<int>(row));
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", static_cast<int>(row));
                            ImGui::TableNextColumn();
                            Entity from{table.from(row), world.generation(table.from(row))};
                            auto name = std::to_string(from.index) + "#" + std::to_string(from.generation);
                            if (world.components(from).has<Name>())
                            {
                                name = world.components(from).get<Name>().value;
                            }
                            if (ImGui::Button(name.c_str()))
                            {
                                selection.entity = from;
                            }
                            ImGui::TableNextColumn();
                            Entity to{table.to(row), world.generation(table.to(row))};
                            name = std::to_string(to.index) + "#" + std::to_string(to.generation);
                            if (world.components(to).has<Name>())
                            {
                                name = world.components(to).get<Name>().value;
                            }
                            if (ImGui::Button(name.c_str()))
                            {
                                selection.entity = to;
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndTable();
                    }

                    if (!continueSelectedSparseRelationTable)
                    {
                        state.selectedSparseRelationTableId = {};
                    }
                }
                ImGui::PopID();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Resources"))
            {
                if (ImGui::BeginTable("Resources", 2,
                                      ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                                          ImGuiTableFlags_BordersOuter))
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Is Present");
                    ImGui::TableHeadersRow();
                    for (const auto& [type, name] : world.types().resources())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Button(name.c_str()))
                        {
                            state.selectedResourceType = type;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", world.hasResource(*type) ? "true" : "false");
                    }

                    ImGui::EndTable();
                }

                ImGui::PushID("Selected Resource");
                bool continueSelectedResource = true;
                if (state.selectedResourceType != nullptr &&
                    ImGui::CollapsingHeader("Selected Resource", &continueSelectedResource,
                                            ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text("Type: %s", state.selectedResourceType->name().c_str());
                    ImGui::Separator();

                    if (world.hasResource(*state.selectedResourceType))
                    {
                        inspector.show("Value", *state.selectedResourceType,
                                       world.resource(*state.selectedResourceType));
                    }
                    else
                    {
                        ImGui::Text("<not present>");
                    }

                    if (!continueSelectedResource)
                    {
                        state.selectedResourceType = nullptr;
                    }
                }
                ImGui::PopID();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();

            ImGui::End();
        });
}
