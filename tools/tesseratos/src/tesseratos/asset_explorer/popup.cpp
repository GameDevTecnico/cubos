#include <imgui.h>

#include <cubos/core/reflection/type.hpp>

#include <tesseratos/asset_explorer/popup.hpp>

using cubos::core::reflection::Type;

using cubos::engine::AnyAsset;
using cubos::engine::Asset;
using cubos::engine::Assets;

using namespace tesseratos;

bool assetSelectionPopup(const std::string& title, AnyAsset& selectedAsset, const Type& type, const Assets& assets)
{
    bool returnValue = false;

    ImGui::PushID(title.c_str());
    ImGui::OpenPopup(title.c_str());
    if (ImGui::BeginPopupModal(title.c_str(), nullptr))
    {
        for (auto const& asset : assets.listAll())
        {
            if (&assets.type(asset) == &type)
            {
                auto path = assets.readMeta(asset)->get("path");
                ImGui::Text("- %s", path->c_str());
                ImGui::SameLine();
                if (ImGui::Button("Select"))
                {
                    selectedAsset = asset;
                    returnValue = true;
                }
            }
        }
        ImGui::EndPopup();
    }

    if (returnValue)
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::PopID();
    return returnValue;
}
