#include <memory>
#include <unordered_map>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include <tesseratos/asset_explorer/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/scene_editor/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::Blueprint;
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
        std::vector<std::pair<std::string, std::unique_ptr<SceneInfo>>> scenes;
    };

    // Resource which stores the editor state.
    struct State
    {
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

    for (auto& [name, subscene] : scene.scenes)
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
        for (auto& [sname, subscene] : scene.scenes)
        {
            if (sname == subsceneName)
            {
                placeEntity(name.substr(split + 1), handle, *subscene);
                return;
            }
        }
        auto subScene = std::make_unique<SceneInfo>();
        subScene->name = subsceneName;
        auto& subSceneHandle = scene.scenes.emplace_back(subsceneName, std::move(subScene)).second;
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
                CUBOS_INFO("Removing entity '{}' from scene '{}'", name, scene.name);
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

// Recursively draws the scene hierarchy and allows the user to remove scenes
static void showSceneHierarchy(SceneInfo& scene, Commands& cmds, EntitySelector& selector, int hierarchyDepth)
{
    ImGui::PushID(&scene);

    bool nodeOpen;
    bool isTree = false;

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
                    scene.name = buff;
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
                // TODO: actually add a sub scene - should add new scene to state.imports
                std::string sceneName = scene.name + "scene" + std::to_string(scene.scenes.size() + 1);
                auto newSubscene = std::make_unique<SceneInfo>();
                newSubscene->name = sceneName;
                scene.scenes.emplace_back(sceneName, std::move(newSubscene));
            }
        }

        std::vector<std::size_t> sceneToRemove;
        for (std::size_t i = 0; i < scene.scenes.size(); i++)
        {
            if (scene.scenes[i].second->shouldBeRemoved)
            {
                sceneToRemove.push_back(i);
            }
            else
            {
                showSceneHierarchy(*scene.scenes[i].second, cmds, selector, hierarchyDepth + 1);
            }
        }
        for (const auto& i : sceneToRemove)
        {
            // TODO: remove sub-scene from state.imports
            scene.scenes.erase(scene.scenes.begin() + static_cast<std::ptrdiff_t>(i));
        }

        if (hierarchyDepth != 0 && isTree)
        {
            ImGui::TreePop();
        }
    }
    ImGui::PopStyleColor(1);
    ImGui::PopID();
}

static Blueprint intoBlueprint(const World& world, const SceneInfo& info)
{
    Blueprint blueprint{};

    // Add matching entities from the world into the blueprint.
    for (const auto& [name, worldEnt] : info.entities)
    {
        auto blueprintEnt = blueprint.create(name);
        for (auto [type, value] : world.components(worldEnt))
        {
            blueprint.add(blueprintEnt, AnyValue::copyConstruct(*type, value));
        }
    }

    // Merge sub-scene blueprints into it.
    for (const auto& [name, subInfo] : info.scenes)
    {
        blueprint.merge(name, intoBlueprint(world, *subInfo));
    }

    return blueprint;
}

static void saveScene(const World& world, Assets& assets, State& state)
{
    Scene scene{};
    scene.blueprint = intoBlueprint(world, state.root);
    scene.imports = state.imports;
    assets.store(state.asset, std::move(scene));
    assets.save(state.asset);
}

void tesseratos::sceneEditorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::scenePlugin);

    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(assetExplorerPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.addResource<State>();

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
        .tagged("cubos.imgui")
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
                    showSceneHierarchy(state.root, cmds, selector, 0);
                }
            }

            ImGui::End();
        });
}
