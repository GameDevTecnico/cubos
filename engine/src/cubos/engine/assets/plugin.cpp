#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static void init(Write<Assets> assets, Write<Settings> settings)
{
    // Get the relevant settings.
    if (settings->getBool("assets.io.enabled", true))
    {
        std::filesystem::path path = settings->getString("assets.io.path", "assets");
        bool readOnly = settings->getBool("assets.io.readOnly", true);

        // Create a standard archive for the assets directory and mount it.
        FileSystem::mount("/assets", std::make_unique<StandardArchive>(path, true, readOnly));

        // Load the meta files on the assets directory.
        assets->loadMeta("/assets");
    }
}

static void cleanup(Write<Assets> assets)
{
    // TODO: maybe don't do this every frame?
    assets->cleanup();
}

void cubos::engine::assetsPlugin(Cubos& cubos)
{
    cubos.addPlugin(settingsPlugin);

    cubos.addResource<Assets>();

    cubos.startupTag("cubos.assets.init").after("cubos.settings");
    cubos.startupTag("cubos.assets.bridge").after("cubos.assets.init").before("cubos.assets");

    cubos.startupSystem(init).tagged("cubos.assets.init");
    cubos.system(cleanup).tagged("cubos.assets.cleanup");
}
