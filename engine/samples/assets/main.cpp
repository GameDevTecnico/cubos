#include <cubos/core/data/file_system.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::memory::Stream;
using namespace cubos::engine;

class TextBridge : public AssetBridge
{
public:
    bool load(Assets& assets, const AnyAsset& handle) override
    {
        // Get the asset's path - guaranteed to be present by the asset manager.
        auto path = assets.readMeta(handle)->get("path").value();

        // Open the asset's file.
        auto stream = FileSystem::open(path, File::OpenMode::Read);
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
    auto text = assets->read(TextAsset);
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
