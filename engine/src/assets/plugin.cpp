#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::AssetsInitTag);
CUBOS_DEFINE_TAG(cubos::engine::AssetsCleanupTag);
CUBOS_DEFINE_TAG(cubos::engine::AssetsBridgeTag);
CUBOS_DEFINE_TAG(cubos::engine::AssetsTag);

using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;

void cubos::engine::assetsPlugin(Cubos& cubos)
{

    cubos.addPlugin(settingsPlugin);

    cubos.addResource<Assets>();

    cubos.startupTag(AssetsInitTag).after(SettingsTag);
    cubos.startupTag(AssetsBridgeTag).after(AssetsInitTag).before(AssetsTag);

    cubos.startupSystem("load asset meta files").tagged(AssetsInitTag).call([](Assets& assets, Settings& settings) {
        // Get the relevant settings.
        if (settings.getBool("assets.io.enabled", true))
        {
            std::filesystem::path path = settings.getString("assets.io.path", "assets");
            bool readOnly = settings.getBool("assets.io.readOnly", true);

            // Create a standard archive for the assets directory and mount it.
            FileSystem::mount("/assets", std::make_unique<StandardArchive>(path, true, readOnly));

            // Load the meta files on the assets directory.
            assets.loadMeta("/assets");
        }
    });

    cubos.system("cleanup unused assets").tagged(AssetsCleanupTag).call([](Assets& assets) {
        // TODO: maybe don't do this every frame?
        assets.cleanup();
    });
}
