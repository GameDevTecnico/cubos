#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

using cubos::core::ecs::EventReader;
using cubos::core::ecs::EventWriter;

using cubos::engine::AnyAsset;
using cubos::engine::Assets;
using cubos::engine::Cubos;
using cubos::engine::Settings;

using namespace tesseratos;

CUBOS_REFLECT_IMPL(AssetExplorer)
{
    return cubos::core::ecs::TypeBuilder<AssetExplorer>("tesseratos::AssetExplorer").withField("isOpen", &AssetExplorer::isOpen).build();
}

namespace
{
    struct AssetExplorerState
    {
        CUBOS_ANONYMOUS_REFLECT(AssetExplorerState);

        std::vector<AnyAsset> assetsVector;
    };
} // namespace

static bool assetsCompare(AnyAsset const& a, AnyAsset const& b, Assets& assets)
{
    std::string aPath = assets.readMeta(a)->get("path").value();
    std::string bPath = assets.readMeta(b)->get("path").value();

    auto aCount = std::count(aPath.begin(), aPath.end(), '/');
    auto bCount = std::count(bPath.begin(), bPath.end(), '/');

    std::string aDir = aPath;
    std::string bDir = bPath;

    aDir.erase(aDir.rfind('/'));
    bDir.erase(bDir.rfind('/'));

    if (aCount != bCount)
    {
        if (aDir.find(bDir + "/") != std::string::npos)
        {
            return true;
        }

        if (bDir.find(aDir + "/") != std::string::npos)
        {
            return false;
        }
    }

    return aPath.compare(bPath) > 0;
}

static void showAsset(Assets& assets, AnyAsset const& asset, cubos::core::ecs::EventWriter<AssetSelectedEvent> events)
{
    std::string path = assets.readMeta(asset)->get("path").value();
    std::string fileName = path.substr(path.rfind('/') + 1);
    ImGui::PushID(path.c_str());
    ImGui::BulletText("%s", fileName.c_str());
    ImGui::SameLine();
    if (ImGui::Button("Select"))
    {
        events.push(AssetSelectedEvent{asset});
    }
    ImGui::PopID();
}

static std::vector<AnyAsset>::iterator showFolder(Assets& assets, std::string const& folder,
                                                  std::vector<AnyAsset>::iterator iter,
                                                  std::vector<AnyAsset>::iterator end,
                                                  cubos::core::ecs::EventWriter<AssetSelectedEvent> events)
{
    std::string displayName = folder;
    displayName.erase(0, displayName.rfind('/'));
    if (displayName.empty())
    {
        displayName.append("/");
    }

    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(149, 252, 75));
    if (ImGui::TreeNode(displayName.c_str()))
    {
        ImGui::PopStyleColor(1);
        while (iter != end)
        {
            AnyAsset asset = *iter;
            std::string assetPath = assets.readMeta(asset)->get("path").value();
            if (assetPath.find(folder + "/") == std::string::npos)
            {
                break;
            }

            std::string dirless = assetPath;
            dirless.erase(0, folder.length() + 1);
            auto pos = dirless.find('/');
            if (pos != std::string::npos)
            {
                std::string subfolder = folder + "/" + dirless.substr(0, pos);
                iter = showFolder(assets, subfolder, iter, end, events);
            }
            else
            {
                showAsset(assets, *iter, events);
                iter++;
            }
        }
        ImGui::TreePop();
    }
    else
    {
        ImGui::PopStyleColor(1);
        while (iter != end)
        {
            if (assets.readMeta(*iter)->get("path").value().find(folder + "/") == std::string::npos)
            {
                break;
            }
            iter++;
        }
    }
    return iter;
}

static void refresh(Assets& assets, AssetExplorerState& state)
{
    assets.importAll("/assets");

    state.assetsVector.clear();

    for (auto const& a : assets.listAll())
    {
        if (assets.readMeta(a)->get("path").has_value())
        {
            state.assetsVector.push_back(a);
        }
    }
    std::sort(state.assetsVector.begin(), state.assetsVector.end(),
              [&assets](AnyAsset const& a, AnyAsset const& b) { return assetsCompare(a, b, assets); });
}

void tesseratos::assetExplorerPlugin(Cubos& cubos)
{
    cubos.event<AssetSelectedEvent>();

    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::assetsPlugin);

    cubos.resource<AssetExplorer>();
    cubos.resource<AssetExplorerState>();

    cubos.system("show Asset Explorer UI")
        .onlyIf([](AssetExplorer& tool) { return tool.isOpen; })
        .tagged(cubos::engine::imguiTag)
        .call([](Assets& assets, cubos::core::ecs::EventWriter<AssetSelectedEvent> events,
                 AssetExplorerState& state, AssetExplorer& tool) {

            ImGui::Begin("Asset Explorer", &tool.isOpen);

            if (state.assetsVector.empty())
            {
                refresh(assets, state);
            }

            if (ImGui::Button("Refresh"))
            {
                refresh(assets, state);
            }

            showFolder(assets, "", state.assetsVector.begin(), state.assetsVector.end(), events);

            ImGui::End();
        });
}
