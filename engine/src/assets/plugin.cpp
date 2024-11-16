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
        std::string appOsPath = settings.getString("assets.app.osPath", "");
        std::string builtinOsPath = settings.getString("assets.builtin.osPath", "");

        // Mount the application assets directory only if a path was provided.
        if (!appOsPath.empty())
        {
            bool readOnly = settings.getBool("assets.app.readOnly", true);

            // Create a standard archive for the assets directory and mount it.
            auto archive = std::make_unique<StandardArchive>(appOsPath, /*isDirectory=*/true, readOnly);
            if (archive->initialized() && FileSystem::mount("/assets", std::move(archive)))
            {
                CUBOS_INFO("Mounted application assets directory with path {}", appOsPath);
            }
            else
            {
                CUBOS_ERROR("Couldn't mount application assets directory with path {}", appOsPath);
            }
        }

        // Mount the builtin assets directory only if a path was provided.
        if (!builtinOsPath.empty())
        {
            // Create a standard archive for the builtin assets directory and mount it.
            auto archive = std::make_unique<StandardArchive>(builtinOsPath, /*isDirectory=*/true, /*readOnly=*/true);
            if (archive->initialized() && FileSystem::mount("/builtin", std::move(archive)))
            {
                CUBOS_INFO("Mounted builtin assets directory with path {}", builtinOsPath);
            }
            else
            {
                CUBOS_ERROR("Couldn't mount builtin assets directory with path {}", builtinOsPath);
            }
        }

        if (FileSystem::find("/assets") != nullptr)
        {
            assets.loadMeta("/assets");
        }
        else
        {
            CUBOS_WARN("No application assets directory has been mounted, have you forgotten to set the "
                       "`assets.app.osPath` setting?");
        }

        if (FileSystem::find("/builtin") != nullptr)
        {
            assets.loadMeta("/builtin");
        }
        else
        {
            CUBOS_WARN("No builtin assets directory has been mounted, have you forgotten to set the "
                       "`assets.builtin.osPath` setting?");
        }
    });

    cubos.system("cleanup unused assets").tagged(assetsCleanupTag).call([](Assets& assets) {
        // TODO: maybe don't do this every frame?
        assets.cleanup();
    });
}
