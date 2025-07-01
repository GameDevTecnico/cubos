#include "plugin.hpp"

#include <imgui_internal.h>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>

using namespace cubos::engine;
using namespace tesseratos;

CUBOS_REFLECT_IMPL(LayoutState)
{
    return cubos::core::ecs::TypeBuilder<LayoutState>("tesseratos::LayoutState")
        .withField("isReady", &LayoutState::isReady)
        .withField("layout", &LayoutState::layout)
        .build();
}

void tesseratos::layoutPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);

    cubos.resource<LayoutState>();

    cubos.system("setup Dockspace").tagged(imguiTag).call([](LayoutState& state) {
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

        if (!state.isReady && ImGui::GetFrameCount() > 1)
        {
            state.isReady = true;

            // Remove any existing layout
            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

            // Split the dockspace into 3 parts:
            ImGuiID dockMainId = dockspaceId;
            ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.25f, nullptr, &dockMainId);
            ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.25f, nullptr, &dockMainId);

            // Dock windows (assumes these windows exist and are created somewhere else)
            // ImGui::DockBuilderDockWindow("Viewport", dockMainId);
            ImGui::DockBuilderDockWindow("Asset Explorer", dockLeft);
            ImGui::DockBuilderDockWindow("Project", dockBottom);

            ImGui::DockBuilderFinish(dockspaceId);
        }
    });
}
