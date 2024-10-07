#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::assetsInitTag);
CUBOS_DEFINE_TAG(cubos::engine::assetsCleanupTag);
CUBOS_DEFINE_TAG(cubos::engine::assetsBridgeTag);
CUBOS_DEFINE_TAG(cubos::engine::assetsTag);

using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;

void cubos::engine::assetsPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.resource<Assets>();

    cubos.startupTag(assetsTag);
    cubos.startupTag(assetsInitTag).after(settingsTag);
    cubos.startupTag(assetsBridgeTag).after(assetsInitTag).before(assetsTag);

    cubos.tag(assetsCleanupTag);

    cubos.startupSystem("load asset meta files").tagged(assetsInitTag).call([](Assets& assets, Settings& settings) {
        // Get the relevant settings.
        if (settings.getBool("assets.io.enabled", true))
        {
            std::string path = settings.getString("assets.io.path", "assets");
            bool readOnly = settings.getBool("assets.io.readOnly", true);

            // Create a standard archive for the assets directory and mount it.
            auto archive = std::make_unique<StandardArchive>(path, true, readOnly);
            if (archive->initialized())
            {
                CUBOS_INFO("Successfully initialized assets directory archive with path {}", path);
                if (FileSystem::mount("/assets", std::move(archive)))
                {
                    // Load the meta files on the assets directory.
                    assets.loadMeta("/assets");
                }
            }
            else
            {
                CUBOS_ERROR("Couldn't initialize assets directory archive with path {}", path);
            }
        }

        // Create a standard archive for the builtin assets directory and mount it.
        // Also load the meta files on the builtin assets directory.
        std::string builtinPath = settings.getString("assets.builtin.path", CUBOS_ENGINE_ASSETS_PATH);
        auto builtinArchive = std::make_unique<StandardArchive>(builtinPath, true, true);
        if (builtinArchive->initialized())
        {
            CUBOS_INFO("Successfully initialized builtin assets directory archive with path {}", builtinPath);
            if (FileSystem::mount("/builtin", std::move(builtinArchive)))
            {
                assets.loadMeta("/builtin");
            }
        }
        else
        {
            CUBOS_ERROR("Couldn't initialize builtin assets directory archive with path {}", builtinPath);
        }
    });

    cubos.system("cleanup unused assets").tagged(assetsCleanupTag).call([](Assets& assets) {
        // TODO: maybe don't do this every frame?
        assets.cleanup();
    });
}
