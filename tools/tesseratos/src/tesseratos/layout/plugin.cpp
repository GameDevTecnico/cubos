#include "plugin.hpp"

#include <imgui_internal.h>
#include <nlohmann/json.hpp>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>

#include "../menu_bar/plugin.hpp"
#include "layout.hpp"

using namespace cubos::engine;
using namespace tesseratos;

void tesseratos::layoutPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(menuBarPlugin);

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

            auto testLayout = R"(
                {
                    "content": [
                        {
                            "content": [
                                {
                                    "split": {
                                        "ratio": 0.25,
                                        "direction": "left"
                                    },
                                    "content": [
                                        "Asset Explorer"
                                    ]
                                },
                                {
                                    "split": {
                                        "ratio": 0.25,
                                        "direction": "right"
                                    },
                                    "content": [
                                        "Debugger"
                                    ]
                                }
                            ]
                        },
                        {
                            "split": {
                                "ratio": 0.25,
                                "direction": "down"
                            },
                            "content": [
                                "Project",
                                "Importer"
                            ]
                        }
                    ]
                }
            )"_json;

            state.isReady = true;

            // Remove any existing layout
            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

            Layout l1 = Layout::loadFromJson(testLayout);
            l1.applyToImGui(dockspaceId);

            ImGui::DockBuilderFinish(dockspaceId);
        }
    });
}
