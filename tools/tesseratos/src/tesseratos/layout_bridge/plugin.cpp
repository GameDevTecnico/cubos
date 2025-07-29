#include "plugin.hpp"

#include <nlohmann/json.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "../menu_bar/plugin.hpp"

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
    cubos.depends(assetsPlugin);

    cubos.depends(menuBarPlugin);

    cubos.startupSystem("setup bridge to load .layout file").tagged(assetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".layout", std::make_unique<LayoutBridge>());
    });

    static const Asset<std::string> DefaultLayout = AnyAsset("631a8d44-6f73-4479-9b0a-f2b21f4e0cf6");

    cubos.startupSystem("Load docking layout")
        .tagged(assetsTag)
        .call([](const Assets& assets, LayoutState& layoutState) {
            layoutState.layout = assets.read(DefaultLayout).get();
            layoutState.isReady = false;
        });
}