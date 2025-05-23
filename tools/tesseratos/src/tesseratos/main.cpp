#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "menu_bar/plugin.hpp"

using namespace cubos::engine;
using namespace tesseratos;

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(cubos::engine::defaultsPlugin);
    cubos.plugin(cubos::engine::freeCameraPlugin);

    cubos.plugin(menuBarPlugin);

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

    cubos.startupSystem("set ImGui context and enable docking")
        .after(imguiInitTag)
        .call([](ImGuiContextHolder& holder) {
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
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
        ImGui::Begin("Tesseratos", nullptr, windowFlags);
        ImGui::PopStyleVar();
        ImGuiID dockspaceId = ImGui::GetID("TesseratosRoot");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0F, 0.0F));
        ImGui::End();
    });

    cubos.run();
}
