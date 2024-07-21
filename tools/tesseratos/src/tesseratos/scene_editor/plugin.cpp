#include "plugin.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include "../asset_explorer/plugin.hpp"
#include "../asset_explorer/popup.hpp"
#include "../entity_selector/plugin.hpp"
#include "../toolbox/plugin.hpp"

using cubos::core::ecs::Blueprint;
using cubos::core::ecs::convertEntities;
using cubos::core::ecs::EntityHash;
using cubos::core::ecs::EphemeralTrait;
using cubos::core::ecs::Name;
using cubos::core::ecs::World;
using cubos::core::memory::AnyValue;

using cubos::engine::Asset;
using cubos::engine::Assets;
using cubos::engine::Commands;
using cubos::engine::Cubos;
using cubos::engine::Entity;
using cubos::engine::Scene;

using namespace tesseratos;

namespace
{
    // Resource used to store Scene(s) information
    struct SceneInfo
    {
        bool isExpanded{false};
        bool shouldBeRemoved{false};
        std::string name;
        std::vector<std::pair<std::string, Entity>> entities;
        std::vector<std::unique_ptr<SceneInfo>> scenes;
        Asset<Scene> asset;
    };

    // Resource which stores the editor state.
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        Asset<Scene> asset{nullptr};
        SceneInfo root;
        std::unordered_map<std::string, Asset<Scene>> imports;
    };
} // namespace

static void closeScene(Commands& commands, SceneInfo& scene)
{
    for (const auto& [name, entity] : scene.entities)
    {
        commands.destroy(entity);
    }
    scene.entities.clear();

    for (auto& subscene : scene.scenes)
    {
        closeScene(commands, *subscene);
    }
    scene.scenes.clear();
}

static void placeEntity(const std::string& name, Entity handle, SceneInfo& scene)
{
    auto split = name.find('.');
    if (split == std::string::npos)
    {
        scene.entities.emplace_back(name, handle);
    }
    else
    {
        auto subsceneName = name.substr(0, split);
        for (auto& subscene : scene.scenes)
        {
            if (subscene->name == subsceneName)
            {
                placeEntity(name.substr(split + 1), handle, *subscene);
                return;
            }
        }
        auto subScene = std::make_unique<SceneInfo>();
        subScene->name = subsceneName;
        auto& subSceneHandle = scene.scenes.emplace_back(std::move(subScene));
        placeEntity(name.substr(split + 1), handle, *subSceneHandle);
    }
}

static void openScene(const Asset<Scene>& sceneToSpawn, Commands& commands, const Assets& assets, State& state)
{
    const auto& scene = *assets.read(sceneToSpawn);
    state.imports = scene.imports;
    auto builder = commands.spawn(scene.blueprint);
    for (const auto& [entity, name] : scene.blueprint.bimap())
    {
        placeEntity(name, builder.entity(name), state.root);
    }
}

static void loadSubScene(const Asset<Scene>& sceneToSpawn, Commands& commands, const Assets& assets, SceneInfo& scene)
{
    const auto& sceneData = *assets.read(sceneToSpawn);
    auto builder = commands.spawn(sceneData.blueprint);
    for (const auto& [entity, name] : sceneData.blueprint.bimap())
    {
        placeEntity(name, builder.entity(name), scene);
    }
}

// Used to check if input entity names are valid.
static int entityNameFilter(ImGuiInputTextCallbackData* data)
{
    if (data->EventChar == '.')
    {
        return 1;
    }
    return 0;
}

// Shows the entities within a scene and allows the user to select, add more or remove them.
static void showSceneEntities(std::vector<std::pair<std::string, Entity>>& entities, SceneInfo& scene,
                              EntitySelector& selector, Commands& cmds, int hierarchyDepth)
{
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 255, 255));
    //  Add entity to current scene (needs to be root, not a sub scene)
    if (hierarchyDepth == 0 && ImGui::Button("Add Entity"))
    {
        std::string entityName = scene.name + "unnamed" + std::to_string(scene.entities.size() + 1);
        entities.emplace_back(entityName, cmds.create().entity());
    }

    // List entities (that can be removed if it's not a sub scene and selected)
    std::vector<std::size_t> entitiesToRemove;
    for (std::size_t i = 0; i < entities.size(); i++)
    {
        bool removed = false;
        auto name = entities[i].first;
        auto handle = entities[i].second;
        bool selected = selector.selection == handle;
        std::string buff = name;
        std::string id = "##" + name;
        // Gives the selected entity a different color (bright green)
        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(149, 252, 75));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 255, 255));
        }

        ImGui::PushID(&entities[i]);
        ImGui::Selectable(name.c_str(), false, 0);
        if (ImGui::IsItemHovered())
        {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && hierarchyDepth == 0)
            {
                // popup menu to change entity name
                ImGui::OpenPopup("Name Menu");
            }
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                CUBOS_INFO("Selected entity {}", name);
                selector.selection = handle;
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && hierarchyDepth == 0)
            {
                // popup menu to delete entity
                ImGui::OpenPopup(id.c_str());
            }
        }

        if (ImGui::BeginPopup("Name Menu"))
        {
            ImGui::PushID("nameChange");
            ImGui::InputText("", &buff, ImGuiInputTextFlags_CallbackCharFilter, entityNameFilter);
            // set cursor immediately in the box without having to click in it
            ImGui::SetKeyboardFocusHere(-1);

            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
            {
                ImGui::CloseCurrentPopup();
                if (name != buff && !buff.empty())
                {
                    entities[i].first = buff;
                }
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup(id.c_str()))
        {
            if (ImGui::Button("Remove Entity"))
            {
                entitiesToRemove.push_back(i);
                CUBOS_INFO("Removing entity {} from scene {}", name, scene.name);
                cmds.destroy(handle);
                ImGui::EndPopup();
                removed = true;
                ImGui::PopID();
            }
            if (!removed)
            {
                ImGui::EndPopup();
            }
        }

        if (!removed)
        {
            ImGui::PopID();
        }

        ImGui::PopStyleColor(1);
    }

    for (const auto& i : entitiesToRemove)
    {
        entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(i));
    }
    ImGui::PopStyleColor(1);
}

static void updateNameComponent(SceneInfo& scene, Commands& commands)
{
    for (const auto& [name, entity] : scene.entities)
    {
        auto newName = scene.name + "." + name;
        commands.add(entity, Name{.value = newName});
    }
    for (auto& subscene : scene.scenes)
    {
        updateNameComponent(*subscene, commands);
    }
}

// Recursively draws the scene hierarchy and allows the user to remove scenes
static void showSceneHierarchy(SceneInfo& scene, Commands& cmds, EntitySelector& selector, int hierarchyDepth,
                               Assets& assets, State& state)
{
    ImGui::PushID(&scene);

    bool nodeOpen;
    bool isTree = false;
    Asset<Scene> bufferasset;
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(149, 252, 255));
    // Root node scene
    if (hierarchyDepth == 0)
    {
        nodeOpen = true;
        scene.isExpanded = nodeOpen;
    }
    else
    {
        // If the scene has no entities or subscenes, it's not a TreeNode
        if (scene.scenes.empty() && scene.entities.empty())
        {
            // non TreeNode scenes have the identation of a TreeNode
            ImGui::Indent(8.0F);
            ImGui::Selectable(scene.name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            nodeOpen = true;
            ImGui::Unindent(8.0F);
        }
        else
        {
            nodeOpen = ImGui::TreeNode(&scene, "%s", scene.name.c_str());
            scene.isExpanded = nodeOpen;
            isTree = true;
        }
    }

    if (hierarchyDepth == 1)
    {

        if (ImGui::IsItemHovered())
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("Scene Menu");
            }
            else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                ImGui::OpenPopup("Name Menu");
            }
        }
        if (ImGui::BeginPopup("Scene Menu"))
        {
            if (ImGui::Button("Remove Scene"))
            {
                scene.shouldBeRemoved = true;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("Name Menu"))
        {
            std::string buff = scene.name;
            ImGui::PushID("nameChange");
            ImGui::InputText("", &buff, ImGuiInputTextFlags_CallbackCharFilter, entityNameFilter);
            // set cursor immediately in the box without having to click in it
            ImGui::SetKeyboardFocusHere(-1);
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
            {
                if (scene.name != buff && !buff.empty())
                {
                    auto asset = state.imports[scene.name];
                    state.imports.erase(scene.name);
                    state.imports.emplace(buff, asset);
                    scene.name = buff;
                    updateNameComponent(scene, cmds);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopID();
            ImGui::EndPopup();
        }
    }

    if (nodeOpen)
    {
        // Add entity to current scene (needs to be root, not a sub scene)

        showSceneEntities(scene.entities, scene, selector, cmds, hierarchyDepth);
        if (hierarchyDepth == 0)
        {
            ImGui::Separator();
            if (ImGui::Button("Add Scene"))
            {
                ImGui::OpenPopup("Select a scene");
            }
            if (tesseratos::assetSelectionPopup("Select a scene", bufferasset,
                                                cubos::core::reflection::reflect<Scene>(), assets))
            {
                std::string sceneName = std::string(scene.name + "scene" + std::to_string(scene.scenes.size() + 1));
                auto newSubscene = std::make_unique<SceneInfo>();
                newSubscene->asset = bufferasset;
                newSubscene->name = sceneName;
                scene.scenes.emplace_back(std::move(newSubscene));
                state.imports.emplace(sceneName, bufferasset);
                loadSubScene(bufferasset, cmds, assets, *scene.scenes.back());
                updateNameComponent(*scene.scenes.back(), cmds);
            }
        }

        std::vector<std::size_t> sceneToRemove;
        for (std::size_t i = 0; i < scene.scenes.size(); i++)
        {
            if (scene.scenes[i]->shouldBeRemoved)
            {
                sceneToRemove.push_back(i);
            }
            else
            {
                showSceneHierarchy(*scene.scenes[i], cmds, selector, hierarchyDepth + 1, assets, state);
            }
        }
        for (const auto& i : sceneToRemove)
        {
            scene.scenes.erase(scene.scenes.begin() + static_cast<std::ptrdiff_t>(i));
            state.imports.erase(scene.scenes[i]->name);
        }

        if (hierarchyDepth != 0 && isTree)
        {
            ImGui::TreePop();
        }
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
}

static Blueprint intoBlueprint(std::unordered_map<Entity, Entity, EntityHash>& worldToBlueprint, const World& world,
                               const SceneInfo& info)
{
    Blueprint blueprint{};

    // Merge sub-scene blueprints into it.
    for (const auto& subInfo : info.scenes)
    {
        blueprint.merge(subInfo->name, intoBlueprint(worldToBlueprint, world, *subInfo));
    }

    // Add entities from the world into the blueprint.
    for (const auto& [name, entity] : info.entities)
    {
        worldToBlueprint.emplace(entity, blueprint.create(name));
    }

    // Add components from the world into the blueprint.
    for (const auto& [name, worldEnt] : info.entities)
    {
        auto blueprintEnt = blueprint.bimap().atRight(name);
        for (auto [type, value] : world.components(worldEnt))
        {
            if (!type->has<EphemeralTrait>())
            {
                auto component = AnyValue::copyConstruct(*type, value);
                convertEntities(worldToBlueprint, component.type(), component.get());
                blueprint.add(blueprintEnt, std::move(component));
            }
        }
    }

    return blueprint;
}

static void saveScene(const World& world, Assets& assets, State& state)
{
    std::unordered_map<Entity, Entity, EntityHash> worldToBlueprint{};
    Scene scene{};
    scene.blueprint = intoBlueprint(worldToBlueprint, world, state.root);
    scene.imports = state.imports;
    assets.store(state.asset, std::move(scene));
    assets.save(state.asset);
}

void tesseratos::sceneEditorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::assetsPlugin);
    cubos.depends(cubos::engine::scenePlugin);
    cubos.depends(cubos::engine::imguiPlugin);

    cubos.depends(entitySelectorPlugin);
    cubos.depends(assetExplorerPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.system("open Scene Editor on asset selection")
        .call([](cubos::core::ecs::EventReader<AssetSelectedEvent> reader, Commands commands, const Assets& assets,
                 State& state, Toolbox& toolbox) {
            for (const auto& event : reader)
            {
                if (assets.type(event.asset).is<Scene>())
                {
                    toolbox.open("Scene Editor");
                    CUBOS_INFO("Opening scene {}", event.asset);
                    closeScene(commands, state.root);
                    state.asset = event.asset;
                    openScene(state.asset, commands, assets, state);
                }
            }
        });

    cubos.system("show Scene Editor UI")
        .tagged(cubos::engine::imguiTag)
        .call([](const World& world, Assets& assets, Commands cmds, State& state, EntitySelector& selector,
                 Toolbox& toolbox) {
            if (!toolbox.isOpen("Scene Editor"))
            {
                return;
            }

            ImGui::Begin("Scene Editor");
            if (state.asset == nullptr)
            {
                ImGui::Text("No scene opened");
            }
            else
            {
                auto id = uuids::to_string(state.asset.getId());
                ImGui::Text("Editing scene %s", id.c_str());

                if (ImGui::Button("Save"))
                {
                    saveScene(world, assets, state);
                }

                ImGui::SameLine();

                if (ImGui::Button("Close"))
                {
                    state.asset = Asset<Scene>{};
                    closeScene(cmds, state.root);
                }
                else
                {
                    ImGui::Separator();
                    showSceneHierarchy(state.root, cmds, selector, 0, assets, state);
                }
            }

            ImGui::End();
        });
}
