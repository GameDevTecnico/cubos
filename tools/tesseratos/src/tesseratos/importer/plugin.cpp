#include "plugin.hpp"
#include <filesystem>

#include <imgui.h>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/memory/standard_stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/voxels/model.hpp>

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
        std::size_t gridMax = 0;
        std::size_t gridCount = 0;
        std::unordered_map<std::size_t, std::string> gridFilePaths;
        std::string paletteFilePath;

        // Other Imports...
    };
} // namespace

static bool handleImport(Assets& assets, ImportState& state)
{
    // Find the palette asset using the provided file path.
    auto paletteHandle = assets.find(state.paletteFilePath);
    if (paletteHandle.isNull())
    {
        CUBOS_INFO("Failed to get asset handle for paletteFilePath: {}", state.paletteFilePath);
        CUBOS_INFO("Creating new palette asset.");

        // Create a new empty VoxelPalette and assign a handle to it.
        paletteHandle = assets.create(VoxelPalette{});
        assets.writeMeta(paletteHandle)->set("path", state.paletteFilePath);
        assets.save(paletteHandle);
    }

    std::vector<AnyAsset> gridHandles;
    std::vector<std::string> gridPaths;
    gridHandles.resize(state.gridCount);
    gridPaths.resize(state.gridCount);
    for (std::size_t i = 0; i < state.gridCount; ++i)
    {
        if (state.gridFilePaths.contains(i))
        {
            gridPaths[i] = state.gridFilePaths.at(i);
            gridHandles[i] = assets.find(gridPaths[i]);
        }
    }

    {
        // Read the current VoxelModel asset.
        auto model = assets.read<VoxelModel>(state.currentAsset);

        // Write to the palette asset.
        auto palette = assets.write<VoxelPalette>(paletteHandle);

        // Iterate over the grids which will be exported.
        for (std::size_t i = 0; i < state.gridCount; ++i)
        {
            if (state.gridFilePaths.contains(i))
            {
                if (gridHandles[i].isNull())
                {
                    CUBOS_INFO("Failed to get asset handle for gridPath: {}", gridPaths[i]);
                    CUBOS_INFO("Creating new grid asset");

                    // Create a new empty VoxelGrid and assign a handle to it.
                    gridHandles[i] = assets.create(VoxelGrid{});
                    assets.writeMeta(gridHandles[i])->set("path", gridPaths[i]);
                    assets.save(gridHandles[i]);
                }

                // Merge the current model's palette into the main palette.
                palette.get().merge(model.get().palette(), 1.0F);

                {
                    auto grid = assets.write<VoxelGrid>(gridHandles[i]);

                    // Convert the grid using the main palette.
                    if (!grid.get().convert(model.get().palette(), palette.get(), 1.0F))
                    {
                        CUBOS_ERROR("Error converting grid to main palette");
                        return false;
                    }
                } // Release lock on gridHandle in order to save it.

                // Save the grid to the given path.
                CUBOS_INFO("Saving grid to path: {}", gridPaths[i]);
                if (!assets.save(gridHandles[i]))
                {
                    CUBOS_ERROR("Error saving grid to path: {}", gridPaths[i]);
                    return false;
                }
            }
        }
    } // Release lock on paletteHandle in order to save it.

    // Save the palette to the asset system.
    CUBOS_INFO("Saving palette to path: {}", state.paletteFilePath);
    if (!assets.save(paletteHandle))
    {
        CUBOS_ERROR("Error saving palette to asset system");
        return false;
    }

    assets.writeMeta(state.currentAsset)->set("paletteId", uuids::to_string(paletteHandle.getId()));

    for (std::size_t i = 0; i < state.gridCount; ++i)
    {
        if (gridHandles[i].isNull())
        {
            continue;
        }
        assets.writeMeta(state.currentAsset)
            ->set("grid" + std::to_string(i) + "Id", uuids::to_string(gridHandles[i].getId()));
    }

    assets.saveMeta(state.currentAsset);

    CUBOS_INFO("Import completed successfully");
    return true;
}

static void showImport(Assets& assets, cubos::core::ecs::EventReader<AssetSelectedEvent> reader, ImportState& state)
{
    //  Check whether the selected asset has changed.
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
        cubos::core::memory::Opt<AssetRead<VoxelModel>> voxelModelResult;
        if (assetChanged)
        {
            voxelModelResult = assets.tryRead<VoxelModel>(state.currentAsset);
        }
        if (!state.isModelLoaded ||
            assets.readMeta(state.currentAsset)->get("path").value_or("") != state.currentAssetPath)
        {
            if (!voxelModelResult.contains())
            {
                ImGui::Text("Error reading VoxelModel check console for details");
                return;
            }
            state.gridMax = voxelModelResult.value().get().gridCount();
            state.currentAssetPath = assets.readMeta(state.currentAsset)->get("path").value_or("");
            state.isModelLoaded = true;
        }

        // Handle VoxelModel import options
        ImGui::Text("Import options for VoxelModel: %s",
                    assets.readMeta(state.currentAsset)->get("path").value_or("Unknown").c_str());

        ImGui::Text("Palette File Path:");
        char paletteBuffer[256] = "";
        strncpy(paletteBuffer, state.paletteFilePath.c_str(), sizeof(paletteBuffer) - 1);
        if (strcmp(paletteBuffer, "") == 0)
        {
            auto paletteId = assets.readMeta(state.currentAsset)->get("paletteId");
            if (paletteId.has_value())
            {
                AnyAsset paletteHandle{paletteId.value()};
                auto palettePath = assets.readMeta(paletteHandle)->get("path");
                strncpy(paletteBuffer, palettePath.value().c_str(), sizeof(paletteBuffer) - 1);
            }
        }
        if (ImGui::InputTextWithHint("##paletteFilePath", "Ex:/assets/models/main.pal", paletteBuffer,
                                     sizeof(paletteBuffer)))
        {
            state.paletteFilePath = paletteBuffer;
        }

        ImGui::Spacing();

        ImGui::Text("Number of Grids: Max Supported by file: %lu", state.gridMax);
        int tempGridCount = static_cast<int>(state.gridCount);
        if (ImGui::InputInt("##gridCount", &tempGridCount, 1, 5))
        {
            if (tempGridCount >= 0)
            {
                state.gridCount = static_cast<std::size_t>(tempGridCount);
            }
        }

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
            if (strcmp(inputBuffer, "") == 0)
            {
                auto gridId = assets.readMeta(state.currentAsset)->get("grid" + std::to_string(i) + "Id");
                if (gridId.has_value())
                {
                    AnyAsset gridHandle{gridId.value()};
                    auto gridPath = assets.readMeta(gridHandle)->get("path");
                    strncpy(inputBuffer, gridPath.value().c_str(), sizeof(inputBuffer) - 1);
                }
            }
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
