#include "plugin.hpp"
#include <filesystem>

#include <imgui.h>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/memory/standard_stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/voxels/voxel_model.hpp>

#include "../asset_explorer/plugin.hpp"
#include "../toolbox/plugin.hpp"

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::ecs::EventReader;

using cubos::engine::AnyAsset;
using cubos::engine::AssetRead;
using cubos::engine::Assets;
using cubos::engine::Cubos;
using cubos::engine::VoxelGrid;
using cubos::engine::VoxelModel;
using cubos::engine::VoxelPalette;

namespace fs = std::filesystem;
namespace memory = cubos::core::memory;

using namespace tesseratos;

namespace
{
    struct ImportState
    {
        CUBOS_ANONYMOUS_REFLECT(ImportState);

        // Generic
        AnyAsset currentAsset;
        std::string currentAssetPath;
        bool showError = false;
        bool isModelLoaded = false;

        // VoxelModel specifics
        int gridMax = 0;
        int gridCount = 0;
        std::unordered_map<std::size_t, std::string> gridFilePaths;
        std::string paletteFilePath;

        // Other Imports...
    };
} // namespace

static bool handleImport(Assets& assets, ImportState& state)
{
    CUBOS_INFO("Starting handleImport");
    auto paletteHandle = assets.getAsset(state.paletteFilePath);
    {
        auto model = assets.read<VoxelModel>(state.currentAsset);
        if (paletteHandle.isNull())
        {
            CUBOS_INFO("Failed to get asset handle for paletteFilePath: {}", state.paletteFilePath);
            CUBOS_INFO("Creating new palette asset.");
            // Should create new palette asset.
            paletteHandle = assets.create(VoxelPalette{});
            assets.writeMeta(paletteHandle)->set("path", state.paletteFilePath);
            assets.save(paletteHandle);
            if (paletteHandle.isNull())
            {
                // Should not happen.
                CUBOS_ERROR("Failed to create palette asset.");
                return false;
            }
        }
        {
            auto palette = assets.write<VoxelPalette>(paletteHandle);
            // Iterate over the grids which will be exported.
            for (uint16_t i = 0; i < model.get().getMatricesSize(); ++i)
            {
                if (state.gridFilePaths.contains(i))
                {
                    auto gridPath = state.gridFilePaths.at(i);
                    auto gridHandle = assets.getAsset(gridPath);
                    if (gridHandle.isNull())
                    {
                        CUBOS_INFO("Failed to get asset handle for gridPath: {}", gridPath);
                        CUBOS_INFO("Creating new grid asset.");
                        gridHandle = assets.create(VoxelGrid{});
                        assets.writeMeta(gridHandle)->set("path", gridPath);
                        assets.save(gridHandle);
                        if (gridHandle.isNull())
                        {
                            // Should not happen.
                            CUBOS_ERROR("Failed to create grid asset.");
                            return false;
                        }
                    }
                    // Merge this palette with the main one.
                    palette.get().merge(model.get().getPalette(), 1.0F);

                    {
                        auto grid = assets.write<VoxelGrid>(gridHandle);
                        // Convert the grid to the main palette.
                        if (!grid.get().convert(model.get().getPalette(), palette.get(), 1.0F))
                        {
                            CUBOS_ERROR("Error converting grid to main palette");
                            return false;
                        }
                    }

                    CUBOS_INFO("Saving grid to path: {}", gridPath);

                    // Save the grid to the given path.
                    if (!assets.save(gridHandle))
                    {
                        CUBOS_ERROR("Error saving grid to path: {}", gridPath);
                        return false;
                    }

                    // CUBOS_INFO("Setting grid path to asset meta");
                    //  Not sure if needed (would be helpful probably). Causes deadlock for now.
                    //  assets.writeMeta(state.currentAsset)->set("grid_path", gridPath);
                }
            }
        }
    }

    CUBOS_INFO("Saving palette to path: {}", state.paletteFilePath);

    // Save the palette to the asset system.
    if (!assets.save(paletteHandle))
    {
        CUBOS_ERROR("Error saving palette to asset system");
        return false;
    }

    // CUBOS_INFO("Setting palette path to asset meta");
    //  Not sure if needed (would be helpful probably).
    //  assets.writeMeta(state.currentAsset)->set("palette_path", state.paletteFilePath);

    // Save the model to the asset system.
    if (!assets.save(state.currentAsset))
    {
        CUBOS_ERROR("Error saving model to asset system");
        return false;
    }

    CUBOS_INFO("handleImport completed successfully");
    return true;
}

static void showImport(Assets& assets, cubos::core::ecs::EventReader<AssetSelectedEvent> reader, ImportState& state)
{
    // The instance where the asset has changed.
    bool assetChanged = false;
    for (const auto& event : reader)
    {
        if (event.asset != state.currentAsset)
        {
            state.currentAsset = event.asset;
            assetChanged = true;
            state.isModelLoaded = false;
        }
    }
    if (state.currentAsset == nullptr)
    {
        ImGui::Text("No asset selected");
        state.isModelLoaded = false;
        return;
    }
    if (assets.type(state.currentAsset).is<VoxelModel>())
    {
        std::optional<std::reference_wrapper<const VoxelModel>> voxelModelResult;
        if (assetChanged)
        {
            voxelModelResult = assets.tryRead<VoxelModel>(state.currentAsset);
        }
        if (!state.isModelLoaded ||
            assets.readMeta(state.currentAsset)->get("path").value_or("") != state.currentAssetPath)
        {
            if (!voxelModelResult.has_value())
            {
                ImGui::Text("Error reading VoxelModel check console for details");
                return;
            }
            state.gridMax = voxelModelResult.value().get().getMatricesSize();
            state.currentAssetPath = assets.readMeta(state.currentAsset)->get("path").value_or("");
            state.isModelLoaded = true;
        }

        // Handle VoxelModel import options
        ImGui::Text("Import options for VoxelModel: %s",
                    assets.readMeta(state.currentAsset)->get("path").value_or("Unknown").c_str());

        ImGui::Text("Palette File Path:");
        char paletteBuffer[256] = "";
        strncpy(paletteBuffer, state.paletteFilePath.c_str(), sizeof(paletteBuffer) - 1);
        if (ImGui::InputTextWithHint("##paletteFilePath", "Ex:/assets/models/main.pal", paletteBuffer,
                                     sizeof(paletteBuffer)))
        {
            state.paletteFilePath = paletteBuffer;
        }

        ImGui::Spacing();

        ImGui::Text("Number of Grids: Max Supported by file: %d", state.gridMax);
        ImGui::InputInt("##gridCount", &state.gridCount, 1, 5);

        if (state.gridCount < 1)
        {
            state.gridCount = 1;
        }
        else if (state.gridCount > state.gridMax)
        {
            state.gridCount = state.gridMax;
        }

        for (unsigned long i = 0; i < static_cast<unsigned long>(state.gridCount); ++i)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "Grid File Path %lu:", i + 1);
            ImGui::Text("%s", buffer);

            char inputBuffer[256];
            auto it = state.gridFilePaths.find(i);
            if (it == state.gridFilePaths.end())
            {
                state.gridFilePaths[i] = "";
            }

            std::string& gridPath = state.gridFilePaths[i];
            strncpy(inputBuffer, gridPath.c_str(), sizeof(inputBuffer) - 1);
            if (ImGui::InputTextWithHint(("##gridFilePath" + std::to_string(i)).c_str(), "Ex:/assets/models/car.grd",
                                         inputBuffer, sizeof(inputBuffer)))
            {
                gridPath = inputBuffer;
            }

            ImGui::Spacing();
        }

        if (ImGui::Button("Import"))
        {
            state.showError = (state.paletteFilePath[0] == '\0');
            for (const auto& [key, path] : state.gridFilePaths)
            {
                if (path.empty())
                {
                    state.showError = true;
                    break;
                }
            }
            if (!state.showError)
            {
                handleImport(assets, state);
            }
        }

        if (state.showError)
        {
            ImGui::TextColored({1.0F, 0.0F, 0.0F, 1.0F}, "Both Grid File Name and Palette File Name must be filled.");
        }
    }
    else
    {
        // Handle other asset types
        ImGui::Text("No import options for: %s",
                    assets.readMeta(state.currentAsset)->get("path").value_or("Unknown").c_str());
    }
}

void tesseratos::importerPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::assetsPlugin);
    cubos.depends(toolboxPlugin);
    cubos.resource<ImportState>();

    cubos.system("show Importer UI")
        .tagged(cubos::engine::imguiTag)
        .call([](Assets& assets, Toolbox& toolbox, cubos::core::ecs::EventReader<AssetSelectedEvent> reader,
                 ImportState& state) {
            if (!toolbox.isOpen("Importer"))
            {
                return;
            }

            ImGui::Begin("Importer");

            showImport(assets, reader, state);

            ImGui::End();
        });
}
