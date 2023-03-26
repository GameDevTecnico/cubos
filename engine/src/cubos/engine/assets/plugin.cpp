#include <cubos/engine/assets/plugin.hpp>

#include <cubos/core/settings.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>

using namespace cubos::engine;
using namespace cubos::core;

static void init(Assets& assets, const Settings& settings)
{
    // Get the relevant settings.
    std::filesystem::path path = settings.getString("assets.path", "assets/");
    bool readOnly = settings.getBool("assets.readOnly", true);

    // Create a standard archive for the assets directory and mount it.
    auto archive = std::make_shared<data::STDArchive>(path, true, readOnly);
    data::FileSystem::mount("/assets/", archive);

    // Load the meta files on the assets directory.
    assets.loadMeta("/assets/");
}

static void cleanup(Assets& assets)
{
    // TODO: maybe don't do this every frame?
    assets.cleanup();
}

void cubos::engine::assetsPlugin(Cubos& cubos)
{
    cubos.addResource<Assets>();

    cubos.startupTag("cubos.assets.init").afterTag("cubos.settings");
    cubos.startupTag("cubos.assets.bridge").afterTag("cubos.assets.init").beforeTag("cubos.assets");

    cubos.startupSystem(init).tagged("cubos.assets.init");
    cubos.system(cleanup).tagged("cubos.assets.cleanup");
}
