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
    
    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.plugin(selectionPlugin);

    cubos.plugin(debuggerPlugin);
    cubos.plugin(assetExplorerPlugin);

    cubos.plugin(projectPlugin);

    cubos.plugin(sceneEditorPlugin);
    cubos.plugin(voxelPaletteEditorPlugin);
    cubos.plugin(importerPlugin);

    cubos.system("setup Menu Bar")
        .tagged(imguiTag)
        .call([](AssetExplorerTool& assetExplorerTool, DebuggerTool& debuggerTool,ProjectTool& projectTool,
            ImporterTool& importerTool, SceneEditorTool& sceneEditorTool, VoxelPalleteEditorTool& voxelPalleteEditorTool) {

    ImGui::Begin("Tesseratos");

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Project"))
            {
                ImGui::MenuItem("New...", "Ctrl+N");
                ImGui::MenuItem("Open...", "Ctrl+O");
                ImGui::MenuItem("Browse in Bridge...", "Alt+Ctrl+O");
                ImGui::MenuItem("Open As...", "Alt+Shift+Ctrl+O");
                if(ImGui::BeginMenu("Open Recent")) {
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
                ImGui::MenuItem("Asset Explorer", "", &assetExplorerTool.isOpen);
                ImGui::MenuItem("Debugger", "", &debuggerTool.isOpen);
                ImGui::MenuItem("Importer", "", &importerTool.isOpen);
                ImGui::MenuItem("Project", "", &projectTool.isOpen);
                ImGui::MenuItem("Scene Editor", "", &sceneEditorTool.isOpen);
                ImGui::MenuItem("Voxel Pallete Editor", "", &voxelPalleteEditorTool.isOpen);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

    ImGui::End();
    });
}
