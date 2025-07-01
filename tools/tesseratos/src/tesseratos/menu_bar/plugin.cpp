#include "plugin.hpp"

#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>

#include "../asset_explorer/plugin.hpp"
#include "../debugger/plugin.hpp"
#include "../importer/plugin.hpp"
#include "../project/plugin.hpp"
#include "../scene_editor/plugin.hpp"
#include "../voxel_palette_editor/plugin.hpp"

using namespace cubos::engine;
using namespace tesseratos;

void tesseratos::menuBarPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);

    cubos.plugin(selectionPlugin);

    cubos.plugin(debuggerPlugin);
    cubos.plugin(assetExplorerPlugin);

    cubos.plugin(projectPlugin);

    cubos.plugin(sceneEditorPlugin);
    cubos.plugin(voxelPaletteEditorPlugin);
    cubos.plugin(importerPlugin);

    cubos.system("setup Menu Bar")
        .tagged(imguiTag)
        .call([](AssetExplorer& assetExplorer, Debugger& debugger, Project& project, Importer& importer,
                 SceneEditor& sceneEditor, VoxelPalleteEditor& voxelPalleteEditor) {
            ImGui::Begin("Tesseratos");

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Project"))
                {
                    ImGui::MenuItem("New...", "Ctrl+N");
                    ImGui::MenuItem("Open...", "Ctrl+O");
                    ImGui::MenuItem("Browse in Bridge...", "Alt+Ctrl+O");
                    ImGui::MenuItem("Open As...", "Alt+Shift+Ctrl+O");
                    if (ImGui::BeginMenu("Open Recent"))
                    {
                        ImGui::Text("No recent files...");
                        ImGui::EndMenu();
                    }
                    ImGui::Separator();

                    ImGui::MenuItem("Close", "Ctrl+W");
                    ImGui::MenuItem("Close All", "Alt+Ctrl+W");
                    ImGui::Separator();

                    ImGui::MenuItem("Save", "Ctrl+S");
                    ImGui::MenuItem("Save As", "Shift+Ctrl+S");
                    ImGui::MenuItem("Revert", "F12");
                    ImGui::Separator();

                    ImGui::MenuItem("Export");
                    ImGui::MenuItem("Generate");
                    ImGui::MenuItem("Share");
                    ImGui::Separator();

                    ImGui::MenuItem("Print...", "Ctrl+P");
                    ImGui::MenuItem("Print One Copy", "Ctrl+P");
                    ImGui::Separator();

                    ImGui::MenuItem("File Info...", "Alt+Shift+Ctrl+1");
                    ImGui::MenuItem("Exit", "Ctrl+Q");

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    ImGui::MenuItem("Fullscreen");
                    ImGui::MenuItem("Padding");
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Asset Explorer", "", &assetExplorer.isOpen);
                    ImGui::MenuItem("Debugger", "", &debugger.isOpen);
                    ImGui::MenuItem("Importer", "", &importer.isOpen);
                    ImGui::MenuItem("Project", "", &project.isOpen);
                    ImGui::MenuItem("Scene Editor", "", &sceneEditor.isOpen);
                    ImGui::MenuItem("Voxel Pallete Editor", "", &voxelPalleteEditor.isOpen);

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
        });
}
