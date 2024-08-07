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
        AnyAsset currentAsset;
        int gridCount = 1;
        std::unordered_map<std::size_t, std::string> gridFilePaths;
        char paletteFilePath[256] = "";
        bool showError = false;
    };
} // namespace

/// Tries to load a VoxelModel from the given path.
/// @param path The path of the VoxelModel.
/// @param model The model to fill.
static bool loadVoxelModel(const fs::path& path, VoxelModel& model)
{
    std::string fullPath = "../../tools/tesseratos" + path.string();
    auto* file = fopen(fullPath.c_str(), "rb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!model.loadFrom(stream))
    {
        CUBOS_ERROR("Failed to deserialize model.");
        return false;
    }
    return true;
}

/// Tries to load the palette from the given path.
/// @param path The path of the palette.
/// @param palette The palette to fill.
static bool loadPalette(const fs::path& path, VoxelPalette& palette)
{
    std::string fullPath = "../../tools/tesseratos" + path.string();
    auto* file = fopen(fullPath.c_str(), "rb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!palette.loadFrom(stream))
    {
        CUBOS_ERROR("Failed to deserialize palette.");
        return false;
    }

    return true;
}

/// Saves the given palette to the given path.
/// @param path The path of the palette.
/// @param palette The palette to save.
static bool savePalette(const fs::path& path, const VoxelPalette& palette)
{
    std::string fullPath = "../../tools/tesseratos" + path.string();
    auto* file = fopen(fullPath.c_str(), "wb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!palette.writeTo(stream))
    {
        CUBOS_ERROR("Failed to serialize palette.");
        return false;
    }

    return true;
}

/// Saves the given grid to the given path.
/// @param path The path of the grid.
/// @param grid The grid to export.
static bool saveGrid(const fs::path& path, const VoxelGrid& grid)
{
    std::string fullPath = "../../tools/tesseratos" + path.string();
    auto* file = fopen(fullPath.c_str(), "wb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!grid.writeTo(stream))
    {
        CUBOS_ERROR("Failed to serialize grid.");
        return false;
    }

    return true;
}

static bool handleImport(ImportState& state, VoxelModel& model)
{
    // First, load the palette.
    VoxelPalette palette;
    if (strlen(state.paletteFilePath) > 0)
    {
        if (!loadPalette(state.paletteFilePath, palette))
        {
            CUBOS_WARN("Failed to load palette: write disabled & file {} not found. Going to create new one",
                       state.paletteFilePath);
        }
    }

    // Iterate over the grids which will be exported.
    for (uint16_t i = 0; i < model.getMatricesSize(); ++i)
    {
        if (state.gridFilePaths.contains(i))
        {
            auto modelsPalette = model.getPalette();
            auto modelsGrid = model.getGrid(i);
            auto path = state.gridFilePaths.at(i);
            std::string fullPath = "../../tools/tesseratos" + path;
            if (std::filesystem::exists(fullPath))
            {
                CUBOS_WARN("Output file already exists re-Importing.");
            }

            // If write is enabled, merge this palette with the main one.
            palette.merge(modelsPalette, 1.0F);

            // Convert the grid to the main palette.
            if (!modelsGrid.convert(modelsPalette, palette, 1.0F))
            {
                CUBOS_ERROR("Error converting grid to main palette");
                return false;
            }

            // Save the grid to the given path.
            if (!saveGrid(path, modelsGrid))
            {
                CUBOS_ERROR("Failed to save grid {} to {} .", i, path);
                return false;
            }
        }
    }

    if (!savePalette(state.paletteFilePath, palette))
    {
        CUBOS_ERROR("Failed to save palette to {} .", state.paletteFilePath);
        return false;
    }

    return true;
}

static void showImport(Assets& assets, cubos::core::ecs::EventReader<AssetSelectedEvent> reader, ImportState& state)
{
    for (const auto& event : reader)
    {
        state.currentAsset = event.asset;
    }

    if (state.currentAsset == nullptr)
    {
        ImGui::Text("No asset selected");
    }
    else if (assets.type(state.currentAsset).is<VoxelModel>())
    {
        VoxelModel model;
        if (!loadVoxelModel(fs::path(assets.readMeta(state.currentAsset)->get("path").value()), model))
        {
            ImGui::TextColored({1.0F, 0.0F, 0.0F, 1.0F}, "Failed to load VoxelModel.");
            return;
        }

        // Handle VoxelModel import options
        ImGui::Text("Import options for VoxelModel: %s",
                    assets.readMeta(state.currentAsset)->get("path").value_or("Unknown").c_str());

        ImGui::Text("Palette File Path:");
        ImGui::InputTextWithHint("##paletteFilePath", "Ex:/assets/models/main.pal", state.paletteFilePath,
                                 IM_ARRAYSIZE(state.paletteFilePath));

        ImGui::Spacing();

        ImGui::Text("Number of Grids: Max Supported by file: %d", model.getMatricesSize());
        ImGui::InputInt("##gridCount", &state.gridCount, 1, 5);

        if (state.gridCount < 1)
        {
            state.gridCount = 1;
        }
        else if (state.gridCount > model.getMatricesSize())
        {
            state.gridCount = model.getMatricesSize();
        }

        for (int i = 0; i < state.gridCount; ++i)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "Grid File Path %d:", i + 1);
            ImGui::Text(buffer);

            char inputBuffer[256];
            auto it = state.gridFilePaths.find(i);
            if (it == state.gridFilePaths.end())
            {
                state.gridFilePaths[i] = "";
            }

            std::string& gridPath = state.gridFilePaths[i];
            strncpy(inputBuffer, gridPath.c_str(), sizeof(inputBuffer));
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
                handleImport(state, model);
            }
        }

        if (state.showError)
        {
            ImGui::TextColored({1.0F, 0.0F, 0.0F, 1.0F}, "Both Grid File Name and Palette File Name must be filled.");
        }
    }
    else
    {
        // Handle other asset types (future-proofing)
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
