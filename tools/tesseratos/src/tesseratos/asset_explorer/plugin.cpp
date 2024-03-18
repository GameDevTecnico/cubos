#include <imgui.h>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

#include <tesseratos/asset_explorer/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::EventWriter;

using cubos::engine::AnyAsset;
using cubos::engine::Assets;
using cubos::engine::Cubos;
using cubos::engine::Settings;

using namespace tesseratos;

static bool assetsCompare(AnyAsset const& a, AnyAsset const& b, Assets const& assets)
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

static void showAsset(Assets const& assets, AnyAsset const& asset, EventWriter<AssetSelectedEvent> events)
{
    std::string path = assets.readMeta(asset)->get("path").value();
    ImGui::PushID(path.c_str());
    ImGui::BulletText("%s", path.erase(0, path.rfind('/') + 1).c_str());
    ImGui::SameLine();
    if (ImGui::Button("Select"))
    {
        events.push(AssetSelectedEvent{asset});
    }
    ImGui::PopID();
}

static std::vector<AnyAsset>::iterator showFolder(Assets const& assets, std::string const& folder,
                                                  std::vector<AnyAsset>::iterator iter,
                                                  std::vector<AnyAsset>::iterator end,
                                                  EventWriter<AssetSelectedEvent> events)
{
    std::string displayName = folder;
    displayName.erase(0, displayName.rfind('/'));

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

void tesseratos::assetExplorerPlugin(Cubos& cubos)
{
    cubos.event<AssetSelectedEvent>();

    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::assetsPlugin);
    cubos.depends(toolboxPlugin);

    cubos.system("show Asset Explorer UI")
        .tagged(cubos::engine::imguiTag)
        .call([](const Assets& assets, Settings& settings, Toolbox& toolbox, EventWriter<AssetSelectedEvent> events) {
            if (!toolbox.isOpen("Asset Explorer"))
            {
                return;
            }

            ImGui::Begin("Asset Explorer");

            std::string folder = settings.getString("assets.io.path", "");

            folder.erase(0, folder.rfind('/'));

            std::vector<AnyAsset> assetsVector;
            for (auto const& a : assets.listAll())
            {
                if (assets.readMeta(a)->get("path").has_value())
                {
                    assetsVector.push_back(a);
                }
            }
            std::sort(assetsVector.begin(), assetsVector.end(),
                      [&assets](AnyAsset const& a, AnyAsset const& b) { return assetsCompare(a, b, assets); });

            showFolder(assets, folder, assetsVector.begin(), assetsVector.end(), events);

            ImGui::End();
        });
}
