#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::FileSystem;
using cubos::core::data::STDArchive;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static void init(Write<Assets> assets, Read<Settings> settings)
{
    // Get the relevant settings.
    if (settings->getBool("assets.io.enabled", true))
    {
        std::filesystem::path path = settings->getString("assets.io.path", "assets/");
        bool readOnly = settings->getBool("assets.io.readOnly", true);

        // Create a standard archive for the assets directory and mount it.
        auto archive = std::make_shared<STDArchive>(path, true, readOnly);
        FileSystem::mount("/assets/", archive);

        // Load the meta files on the assets directory.
        assets->loadMeta("/assets/");
    }
}

static void cleanup(Write<Assets> assets)
{
    // TODO: maybe don't do this every frame?
    assets->cleanup();
}

void cubos::engine::assetsPlugin(Cubos& cubos)
{
    cubos.addResource<Assets>();

    cubos.startupTag("cubos.assets.init").afterTag("cubos.settings");
    cubos.startupTag("cubos.assets.bridge").afterTag("cubos.assets.init").beforeTag("cubos.assets");

    cubos.startupSystem(init).tagged("cubos.assets.init");
    cubos.system(cleanup).tagged("cubos.assets.cleanup");
}
