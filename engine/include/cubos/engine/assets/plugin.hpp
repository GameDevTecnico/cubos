#pragma once

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// Plugin which adds asset management to CUBOS.
    /// If the `assets.path` setting is set to an empty string, no file IO will be done.
    ///
    /// @details
    ///
    /// Settings:
    /// - `assets.path`: path to the assets directory - will be mounted to `/assets/` (default: `assets/`)
    /// - `assets.readOnly`: if true, the assets directory will be mounted as read-only (default: `true`)
    ///
    /// Events:
    /// - `AssetEvent`: emitted when an asset is either loaded, modified or unloaded.
    ///
    /// Resources:
    /// - `Assets`: the asset manager, used to access asset data.
    ///
    /// Startup tags:
    /// - `cubos.assets.init`: initializes the assets manager and loads the meta files (after `cubos.settings`).
    /// - `cubos.assets.bridge`: systes which add bridges to the asset manager should be tagged with this.
    /// - `cubos.assets`: systems which load assets should be tagged with this.
    ///
    /// Tags:
    /// - `cubos.assets.cleanup`: frees any assets no longer in use.
    ///
    /// @param cubos CUBOS. main class.
    void assetsPlugin(Cubos& cubos);
} // namespace cubos::engine
