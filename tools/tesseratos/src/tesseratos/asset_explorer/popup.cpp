#include <imgui.h>

#include <cubos/core/reflection/type.hpp>

#include <tesseratos/asset_explorer/popup.hpp>

using cubos::core::reflection::Type;

using cubos::engine::AnyAsset;
using cubos::engine::Asset;
using cubos::engine::Assets;

bool tesseratos::assetSelectionPopup(const std::string& title, AnyAsset& selectedAsset, const Type& type,
                                     const Assets& assets)
{
    bool returnValue = false;

    if (ImGui::BeginPopup(title.c_str()))
    {
        for (auto const& asset : assets.listAll())
        {
            if (!assets.hasKnownType(asset))
            {
                continue;
            }

            if (&assets.type(asset) == &type)
            {
                auto path = assets.readMeta(asset)->get("path");
                ImGui::PushID(path->c_str());
                ImGui::Text("- %s", path->c_str());
                ImGui::SameLine();
                if (ImGui::Button("Select"))
                {
                    selectedAsset = asset;
                    returnValue = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopID();
            }
        }
        ImGui::EndPopup();
    }

    return returnValue;
}
