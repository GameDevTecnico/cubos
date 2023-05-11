#include <cubos/core/data/file_system.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;
using namespace cubos::core;

class TextBridge : public AssetBridge
{
public:
    bool load(Assets& assets, const AnyAsset& handle) override
    {
        // Get the asset's path - guaranteed to be present by the asset manager.
        auto path = assets.readMeta(handle)->get("path").value();

        // Open the asset's file.
        auto stream = data::FileSystem::open(path, data::File::OpenMode::Read);
        if (stream == nullptr)
        {
            CUBOS_ERROR("Could not find file: {}", path);
            return false;
        }

        // Read the file's contents.
        std::string contents;
        stream->readUntil(contents, nullptr);

        // Store the asset's data.
        assets.store(handle, std::move(contents));
        return true;
    }
};

static const Asset<std::string> TextAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");

static void config(Settings& settings)
{
    settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void bridge(Assets& assets)
{
    // Add a custom bridge to load .txt files.
    assets.registerBridge(".txt", std::make_unique<TextBridge>());
}

static void load(const Assets& assets)
{
    // Access the text asset - will be loaded automatically.
    auto text = assets.read(TextAsset);
    memory::Stream::stdOut.print(*text);
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
