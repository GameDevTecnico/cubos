/// @dir
/// @brief Assets plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup assets-plugin Assets
    /// @ingroup plugins
    /// @brief Adds asset management to CUBOS.
    ///
    /// Settings:
    /// - `assets.io.enabled` _ whether asset I/O should be done (default: `true`).
    /// - `assets.io.path` - path to the assets directory - will be mounted to `/assets/` (default: `assets/`)
    /// - `assets.io.readOnly` - if true, the assets directory will be mounted as read-only (default: `true`)
    ///
    /// Events:
    /// - `AssetEvent` - (TODO) emitted when an asset is either loaded, modified or unloaded.
    ///
    /// Resources:
    /// - `Assets` - the asset manager, used to access asset data.
    ///
    /// Startup tags:
    /// - `cubos.assets.init` - initializes the assets manager and loads the meta files (after `cubos.settings`).
    /// - `cubos.assets.bridge` - systes which add bridges to the asset manager should be tagged with this.
    /// - `cubos.assets` - systems which load assets should be tagged with this.
    ///
    /// Tags:
    /// - `cubos.assets.cleanup` - frees any assets no longer in use.

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class.
    /// @ingroup assets-plugin
    void assetsPlugin(Cubos& cubos);
} // namespace cubos::engine
