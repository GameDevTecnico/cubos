#include <memory>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/ecs/system/commands.hpp>
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

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;

using cubos::engine::Asset;
using cubos::engine::Assets;
using cubos::engine::Cubos;
using cubos::engine::Scene;

using namespace tesseratos;

namespace
{
    // Resource used to store Scene(s) information
    struct SceneInfo
    {
        bool isExpanded{false};
        bool isSelected{false};
        bool shouldBeRemoved{false};
        std::string name;
        std::vector<std::pair<std::string, Entity>> entities;
        std::vector<std::pair<std::string, std::unique_ptr<SceneInfo>>> scenes;
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
    for (const auto& [entity, name] : sceneRead->blueprint.bimap())
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

// Recursively draws the scene hierarchy and allows the user to remove scenes
static void showSceneHierarchy(SceneInfo& scene, Commands& cmds, EntitySelector& selector, int hierarchyDepth)
{
    ImGui::PushID(&scene);

    // Root node scene
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

void tesseratos::sceneEditorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::scenePlugin);

    cubos.addPlugin(entitySelectorPlugin);
    cubos.addPlugin(assetExplorerPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.addResource<SceneInfo>();

    cubos.system("open Scene Editor on asset selection")
        .call([](cubos::core::ecs::EventReader<AssetSelectedEvent> reader, Commands commands, const Assets& assets,
                 SceneInfo& scene) {
            for (const auto& event : reader)
            {
                if (assets.type(event.asset).is<Scene>())
                {
                    CUBOS_INFO("Opening scene {}", event.asset);
                    openScene(event.asset, commands, assets, scene);
                }
            }
        });

    cubos.system("show Scene Editor UI")
        .tagged("cubos.imgui")
        .call([](Commands cmds, SceneInfo& scene, EntitySelector& selector, Toolbox& toolbox) {
            if (!toolbox.isOpen("Scene Editor"))
            {
                return;
            }

            ImGui::Begin("Scene Editor");
            showSceneHierarchy(scene, cmds, selector, 0);
            ImGui::End();
        });
}
