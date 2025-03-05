#include "plugin.hpp"

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "../asset_explorer/plugin.hpp"
#include "../debugger/plugin.hpp"
#include "../importer/plugin.hpp"
#include "../project/plugin.hpp"
#include "../scene_editor/plugin.hpp"
#include "../voxel_palette_editor/plugin.hpp"

using cubos::core::memory::Stream;

using namespace cubos::engine;
using namespace tesseratos;

class LayoutBridge : public FileBridge
{
public:
    LayoutBridge()
        : FileBridge(cubos::core::reflection::reflect<std::string>())
    {
    }

    bool loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream) override
    {
        // Dump the file's contents into a string.
        std::string contents;
        stream.readUntil(contents, nullptr);

        // Store the asset's data.
        assets.store(handle, std::move(contents));
        return true;
    }

    bool saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream) override
    {
        // Get the asset's data.
        auto contents = assets.read<std::string>(handle);

        // Write the data to the file.
        stream.print(*contents);
        return true;
    }
};

void tesseratos::layoutBridgePlugin(cubos::engine::Cubos& cubos)
{
    
    // For readability purposes, plugins are divided into blocks based on their dependencies.
    // The first block contains plugins without dependencies.
    // The second block contains plugins which depend on plugins from the first block.
    // The third block contains plugins which depend on plugins from the first and second blocks.
    // And so on.

    cubos.depends(settingsPlugin);
    cubos.depends(assetsPlugin);

    cubos.depends(debuggerPlugin);
    cubos.depends(assetExplorerPlugin);

    cubos.depends(projectPlugin);

    cubos.depends(sceneEditorPlugin);
    cubos.depends(voxelPaletteEditorPlugin);
    cubos.depends(importerPlugin);

    cubos.startupSystem("setup bridge to load .layout file")
        .tagged(assetsBridgeTag)
        .call([](Assets& assets) {
        assets.registerBridge(".layout", std::make_unique<LayoutBridge>());
    });

    static const Asset<std::string> DefaultLayout = AnyAsset("631a8d44-6f73-4479-9b0a-f2b21f4e0cf6");


    cubos.startupSystem("Load docking layout")
        .tagged(assetsTag)
        .call([](const Assets& assets, AssetExplorerTool& assetExplorerTool, DebuggerTool& debuggerTool,
            ProjectTool& projectTool, ImporterTool& importerTool, SceneEditorTool& sceneEditorTool,
            VoxelPalleteEditorTool& voxelPalleteEditorTool) {

        assetExplorerTool.isOpen = true;
        debuggerTool.isOpen = true;
        projectTool.isOpen = true;
        importerTool.isOpen = true;
        sceneEditorTool.isOpen = true;
        voxelPalleteEditorTool.isOpen = false;

        auto layout = assets.read(DefaultLayout).get().c_str();
        ImGui::LoadIniSettingsFromMemory(layout, (size_t)strlen(layout));
    });
}