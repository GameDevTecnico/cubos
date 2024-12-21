#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "asset_explorer/plugin.hpp"
#include "debugger/plugin.hpp"
#include "importer/plugin.hpp"
#include "project/plugin.hpp"
#include "scene_editor/plugin.hpp"
#include "voxel_palette_editor/plugin.hpp"

using namespace cubos::engine;
using namespace tesseratos;

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(cubos::engine::defaultsPlugin);
    cubos.plugin(cubos::engine::freeCameraPlugin);

    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(cubos::engine::toolsPlugin);

    cubos.plugin(debuggerPlugin);
    cubos.plugin(assetExplorerPlugin);

    cubos.plugin(projectPlugin);

    cubos.plugin(sceneEditorPlugin);
    cubos.plugin(voxelPaletteEditorPlugin);
    cubos.plugin(importerPlugin);

    cubos.startupSystem("configure Assets plugin").before(cubos::engine::settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(cubos::engine::assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.startupSystem("set ImGui context and enable docking").after(imguiInitTag).call([](ImGuiContextHolder& holder) {
        ImGui::SetCurrentContext(holder.context);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    });

    cubos.system("setup ImGui dockspace").tagged(imguiTag).call([]() {

        // make DockSpace fullscreen
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // ImGui window flags, check imgui.h for definition
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
        ImGui::Begin("Tesseratos", nullptr, windowFlags);
        
        ImGuiID dockspaceId = ImGui::GetID("TesseratosRoot");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0F, 0.0F));

        ImGui::End();
    });

    cubos.run();
}
