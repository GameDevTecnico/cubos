#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::memory::Stream;

using namespace cubos::engine;

/// [TextBridge]
/// This bridge inherits from the FileBridge, since it will be loading/saving assets from/to single
/// files.
class TextBridge : public FileBridge
{
public:
    TextBridge()
        : FileBridge(cubos::core::reflection::reflect<std::string>())
    {
    }
    /// [TextBridge]

    /// [TextBridge::loadFromFile]
    bool loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream) override
    {
        // Dump the file's contents into a string.
        std::string contents;
        stream.readUntil(contents, nullptr);

        // Store the asset's data.
        assets.store(handle, std::move(contents));
        return true;
    }
    /// [TextBridge::loadFromFile]

    /// [TextBridge::saveToFile]
    bool saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream) override
    {
        // Get the asset's data.
        auto contents = assets.read<std::string>(handle);

        // Write the data to the file.
        stream.print(*contents);
        return true;
    }
};
/// [TextBridge::saveToFile]

int main(int argc, char** argv)
{
    auto cubos = Cubos(argc, argv);

    cubos.plugin(settingsPlugin);
    cubos.plugin(assetsPlugin);

    cubos.startupSystem("configure Assets plugin").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    /// [Registering the bridge]
    cubos.startupSystem("setup bridge to load .txt files").tagged(assetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".txt", std::make_unique<TextBridge>());
    });
    /// [Registering the bridge]

    /// [Loading the asset]
    // Assets are identified through UUIDs which are defined in their .meta files.
    static const Asset<std::string> SampleAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");

    cubos.startupSystem("access .txt asset").tagged(assetsTag).call([](const Assets& assets) {
        // Access the text asset - will be loaded automatically.
        auto text = assets.read(SampleAsset);
        Stream::stdOut.print(*text);
    });
    /// [Loading the asset]

    cubos.run();
    return 0;
}
