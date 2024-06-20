#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "../asset_explorer/plugin.hpp"

using cubos::core::ecs::EventReader;

using cubos::engine::Asset;
using cubos::engine::Assets;
using cubos::engine::Cubos;
using cubos::engine::RenderPalette;
using cubos::engine::Toolbox;
using cubos::engine::VoxelMaterial;
using cubos::engine::VoxelPalette;

using namespace tesseratos;

namespace
{
    struct SelectedPaletteInfo
    {
        CUBOS_ANONYMOUS_REFLECT(SelectedPaletteInfo);

        Asset<VoxelPalette> asset;
        VoxelPalette paletteCopy;
        bool modified;
        Asset<VoxelPalette> next;
    };
} // namespace

CUBOS_REFLECT_IMPL(AssetSelectedEvent)
{
    return cubos::core::ecs::TypeBuilder<AssetSelectedEvent>("tesseratos::AssetSelectedEvent")
        .withField("asset", &AssetSelectedEvent::asset)
        .build();
}

static void savePaletteUiGuard(Assets& assets, SelectedPaletteInfo& selectedPalette)
{
    // The popup only shows when we've already opened a palette and want to show another one.
    // Thus if we haven't set the next asset variable we can just stop here.
    if (selectedPalette.next.isNull())
    {
        return;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

    if (ImGui::BeginPopupModal("Save Palette?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        bool optionSelected = false;

        ImGui::Text("Do you want to save the modified palette?");
        ImGui::Separator();

        if (ImGui::Button("Yes", ImVec2(80, 0)))
        {
            CUBOS_INFO("Saving palette asset {} modifications", selectedPalette.next);
            assets.store(selectedPalette.asset, selectedPalette.paletteCopy);
            assets.save(selectedPalette.asset);
            optionSelected = true;
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(80, 0)))
        {
            CUBOS_DEBUG("Discarding palette asset {} modifications", selectedPalette.next);
            optionSelected = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(80, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        if (optionSelected)
        {
            selectedPalette.asset = assets.load(selectedPalette.next);
            selectedPalette.paletteCopy = assets.read(selectedPalette.asset).get();
            selectedPalette.modified = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void tesseratos::voxelPaletteEditorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::assetsPlugin);
    cubos.depends(cubos::engine::renderVoxelsPlugin);
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::voxelsPlugin);
    cubos.depends(cubos::engine::toolboxPlugin);

    cubos.depends(assetExplorerPlugin);

    cubos.resource<SelectedPaletteInfo>();

    cubos.system("open Voxel Palette Editor on asset selection")
        .tagged(cubos::engine::imguiTag)
        .call([](EventReader<AssetSelectedEvent> reader, Assets& assets, SelectedPaletteInfo& selectedPalette,
                 Toolbox& toolbox) {
            for (const auto& event : reader)
            {
                if (assets.type(event.asset).is<VoxelPalette>())
                {
                    toolbox.open("Palette Editor");

                    CUBOS_INFO("Opening palette asset {}", event.asset);
                    if (!selectedPalette.asset.isNull() && selectedPalette.modified)
                    {
                        CUBOS_DEBUG("Opening save palette UI guard");
                        ImGui::OpenPopup("Save Palette?");
                        selectedPalette.next = event.asset;
                    }
                    else
                    {
                        selectedPalette.asset = assets.load(event.asset);
                        selectedPalette.paletteCopy = assets.read(selectedPalette.asset).get();
                    }
                }
            }

            // When the 'Save Palette?' option is opened using ImGui::OpenPopup, this will be displayed.
            savePaletteUiGuard(assets, selectedPalette);
        });

    cubos.system("show Voxel Palette Editor UI")
        .tagged(cubos::engine::imguiTag)
        .call([](Assets& assets, SelectedPaletteInfo& selectedPalette, RenderPalette& activePalette, Toolbox& toolbox) {
            if (!toolbox.isOpen("Palette Editor"))
            {
                return;
            }

            if (assets.status(selectedPalette.asset) != Assets::Status::Loaded)
            {
                return;
            }

            ImGui::Begin("Palette Editor");

            bool wasMaterialModified = false;
            std::pair<uint16_t, VoxelMaterial> modifiedMaterial;

            for (uint16_t i = 0; i < selectedPalette.paletteCopy.size(); ++i)
            {
                const uint16_t materialIndex = i + 1;
                auto& material = (VoxelMaterial&)selectedPalette.paletteCopy.get(materialIndex);

                std::string label = "Material " + std::to_string(materialIndex);
                if (ImGui::ColorEdit4(label.c_str(), &material.color.r))
                {
                    CUBOS_DEBUG("Modified material");
                    modifiedMaterial = std::pair(materialIndex, material);
                    wasMaterialModified = true;
                }
            }

            if (wasMaterialModified)
            {
                CUBOS_DEBUG("Storing as new asset because palette was modified");
                auto [idx, material] = modifiedMaterial;
                selectedPalette.paletteCopy.set(idx, material);
                selectedPalette.modified = true;
            }

            // Add material / Make Active / Save

            if (ImGui::Button("Add Material") || selectedPalette.paletteCopy.size() == 0)
            {
                selectedPalette.paletteCopy.push(VoxelMaterial{{1.0F, 0.0F, 1.0F, 1.0F}});
                selectedPalette.modified = true;
            }

            ImGui::SameLine();

            if (ImGui::Button("Make Active"))
            {
                activePalette.asset = selectedPalette.asset;
            }

            ImGui::SameLine();

            if (ImGui::Button("Save"))
            {
                assets.store(selectedPalette.asset, selectedPalette.paletteCopy);
                assets.save(selectedPalette.asset);
                selectedPalette.modified = false;
            }

            if (activePalette.asset == selectedPalette.asset)
            {
                ImGui::TextColored({0, 255, 0, 255}, "This is your current active palette!");
            }

            ImGui::End();
        });
}
