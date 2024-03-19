/// @dir
/// @brief @ref assets-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup assets-plugin

#pragma once

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup assets-plugin Assets
    /// @ingroup engine
    /// @brief Adds asset management to @b CUBOS.
    ///
    /// ## Settings
    /// - `assets.io.enabled` - whether asset I/O should be done (default: `true`).
    /// - `assets.io.path` - path to the assets directory - will be mounted to `/assets/` (default: `assets/`).
    /// - `assets.io.readOnly` - if true, the assets directory will be mounted as read-only (default: `true`).
    ///
    /// ## Events
    /// - @ref AssetEvent - (TODO) emitted when an asset is either loaded, modified or unloaded.
    ///
    /// ## Resources
    /// - @ref Assets - the asset manager, used to access asset data.
    ///
    ///
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Initializes the assets manager and loads the meta files (after @ref settingsTag).
    extern Tag assetsInitTag;

    /// @brief Systems which add bridges to the asset manager should be tagged with this.
    extern Tag assetsBridgeTag;

    /// @brief Frees any assets no longer in use.
    extern Tag assetsCleanupTag;

    /// @brief Startup systems which load assets should be tagged with this.
    extern Tag assetsTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup assets-plugin
    void assetsPlugin(Cubos& cubos);
} // namespace cubos::engine
