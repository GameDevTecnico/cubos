#include <memory>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/data/old/debug_serializer.hpp>
#include <cubos/core/ecs/system/commands.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/tools/asset_explorer/plugin.hpp>
#include <cubos/engine/tools/entity_selector/plugin.hpp>
#include <cubos/engine/tools/scene_editor/plugin.hpp>

using cubos::core::data::old::Debug;

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using tools::AssetSelectedEvent;

/// @brief Resource used to store Scene(s) information
struct SceneInfo
{
    bool isExpanded{false};
    bool isSelected{false};
    bool shouldBeRemoved{false};
    std::string name;
    std::vector<std::pair<std::string, Entity>> entities;
    std::vector<std::pair<std::string, std::unique_ptr<SceneInfo>>> scenes;
};

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
        auto subsceneName = name.substr(0, split - 1);
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

static void openScene(const Asset<Scene>& sceneToSpawn, Commands& commands, const Assets& assets, SceneInfo& scene)
{
    closeScene(commands, scene);
    auto sceneRead = assets.read(sceneToSpawn);
    auto builder = commands.spawn(sceneRead->blueprint);
    for (const auto& [entity, name] : sceneRead->blueprint.getMap())
    {
        placeEntity(name, builder.entity(name), scene);
    }
}

static void checkAssetEventSystem(cubos::core::ecs::EventReader<AssetSelectedEvent> reader, Commands commands,
                                  Read<Assets> assets, Write<SceneInfo> scene)
{
    for (const auto& event : reader)
    {
        if (assets->type(event.asset) == typeid(Scene))
        {
            CUBOS_INFO("Opening scene {}", Debug(event.asset));
            openScene(event.asset, commands, *assets, *scene);
        }
    }
}

/// @brief Used to check if input entity names are valid.
static int entityNameFilter(ImGuiInputTextCallbackData* data)
{
    if (data->EventChar == '.')
    {
        return 1;
    }
    return 0;
}

/// @brief Shows the entities within a scene and allows the user to select, add more or remove them.
static void showSceneEntities(std::vector<std::pair<std::string, Entity>>& entities, SceneInfo& scene,
                              cubos::engine::tools::EntitySelector& selector, Commands& cmds, int hierarchyDepth)
{
    // Add entity to current scene (needs to be root, not a sub scene)
    if (hierarchyDepth == 0 && ImGui::Button("Add Entity"))
    {
        std::string entityName = scene.name + "_entity_" + std::to_string(scene.entities.size() + 1);
        entities.emplace_back(entityName, cmds.create().entity());
    }

    // List entities (that can be removed if it's not a sub scene and selected)
    std::vector<std::size_t> entitiesToRemove;
    for (std::size_t i = 0; i < entities.size(); i++)
    {
        auto name = entities[i].first;
        auto handle = entities[i].second;
        ImGui::PushID(&entities[i]);

        if (hierarchyDepth == 0)
        {
            std::string buff = name;

            ImGui::InputText("", &name, ImGuiInputTextFlags_CallbackCharFilter, entityNameFilter);

            if (name != buff)
            {
                entities[i].first = buff;
            }
        }
        else
        {
            ImGui::BulletText("%s", name.c_str());
        }

        ImGui::Spacing();
        if (ImGui::Button("Select"))
        {
            CUBOS_INFO("Selected entity {}", name);
            selector.selection = handle;
        }
        if (hierarchyDepth == 0)
        {
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                entitiesToRemove.push_back(i);
                CUBOS_INFO("Removing entity '{}' from scene '{}'", name, scene.name);
                cmds.destroy(handle);
            }
        }

        ImGui::PopID();
    }

    for (const auto& i : entitiesToRemove)
    {
        entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

/// @brief recursively draws the scene hierarchy and allows the user to remove scenes
static void showSceneHierarchy(SceneInfo& scene, Commands& cmds, cubos::engine::tools::EntitySelector& selector,
                               int hierarchyDepth)
{
    ImGui::PushID(&scene);

    /// Root node scene
    bool nodeOpen = ImGui::TreeNode(&scene, "%s", scene.name.c_str());
    scene.isExpanded = nodeOpen;

    // Remove scene
    if (hierarchyDepth == 1)
    {
        ImGui::SameLine();
        ImGui::PushID(&scene.shouldBeRemoved);
        if (ImGui::Button("Remove Scene"))
        {
            scene.shouldBeRemoved = true;
        }
        ImGui::PopID();
    }

    if (nodeOpen)
    {
        if (hierarchyDepth == 1)
        {
            std::string buff = scene.name;
            ImGui::PushID("nameChange");
            ImGui::InputText("", &buff, ImGuiInputTextFlags_CallbackCharFilter, entityNameFilter);
            ImGui::PopID();

            if (scene.name != buff)
            {
                scene.name = buff;
            }
        }

        // Add entity to current scene (needs to be root, not a sub scene)
        if (hierarchyDepth == 0)
        {
            if (ImGui::Button("Add Scene"))
            {
                std::string sceneName = scene.name + "_scene_" + std::to_string(scene.scenes.size() + 1);
                auto newSubscene = std::make_unique<SceneInfo>();
                newSubscene->name = sceneName;
                scene.scenes.emplace_back(sceneName, std::move(newSubscene));
            }
            ImGui::SameLine();
        }
        showSceneEntities(scene.entities, scene, selector, cmds, hierarchyDepth);

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
            scene.scenes.erase(scene.scenes.begin() + static_cast<std::ptrdiff_t>(i));
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

static void sceneEditorSystem(Commands cmds, Write<SceneInfo> scene,
                              Write<cubos::engine::tools::EntitySelector> selector)
{
    ImGui::Begin("Scene Editor");
    showSceneHierarchy(*scene, cmds, *selector, 0);
    ImGui::End();
}

void cubos::engine::tools::sceneEditorPlugin(Cubos& cubos)
{
    cubos.addPlugin(scenePlugin);

    cubos.addPlugin(cubos::engine::tools::entitySelectorPlugin);
    cubos.addPlugin(cubos::engine::tools::assetExplorerPlugin);

    cubos.addResource<SceneInfo>();

    cubos.system(checkAssetEventSystem);
    cubos.system(sceneEditorSystem).tagged("cubos.imgui");
}
