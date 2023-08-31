/// @file
/// @brief A sample demonstrating how to create a custom asset bridge.
///
/// This sample demonstrates how we can create a custom asset bridge to load assets of a custom
/// type. In this case, a bridge is created to load text files (.txt) as strings. The bridge is
/// registered with the asset manager, and then a text file is loaded and printed to the console.

#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/assets/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::memory::Stream;
using namespace cubos::engine;

/// This bridge inherits from the FileBridge, since it will be loading/saving assets from/to single
/// files.
class TextBridge : public FileBridge
{
public:
    TextBridge()
        : FileBridge(typeid(std::string))
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

// We specify the same UUID as the one defined in the `.meta` file.
static const Asset<std::string> SampleAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void bridge(Write<Assets> assets)
{
    // Add a custom bridge to load .txt files.
    assets->registerBridge(".txt", std::make_unique<TextBridge>());
}

static void load(Read<Assets> assets)
{
    // Access the text asset - will be loaded automatically.
    auto text = assets->read(SampleAsset);
    Stream::stdOut.print(*text);
}

int main()
{
    auto cubos = Cubos();
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(bridge).tagged("cubos.assets.bridge");
    cubos.startupSystem(load).tagged("cubos.assets");
    cubos.run();
    return 0;
}
