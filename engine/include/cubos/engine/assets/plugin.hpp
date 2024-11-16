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
    /// @brief Adds asset management to @b Cubos
    ///
    /// ## Settings
    /// - `assets.app.osPath` - path to the application assets directory - will be mounted to `/assets/.
    ///   If empty (default), no archive is mounted, and the user is responsible for mounting one.
    /// - `assets.app.readOnly` - whether the mounted archive, if any, should be read-only. Default is `true`.
    /// - `assets.builtin.osPath` - path to the builtin assets directory - will be mounted to `/builtin/`.
    ///   If empty (default), no archive is mounted, and the user is responsible for mounting one.
    ///
    /// ## Resources
    /// - @ref Assets - the asset manager, used to access asset data.
    ///
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Initializes the assets manager and loads the meta files (after @ref settingsTag).
    CUBOS_ENGINE_API extern Tag assetsInitTag;

    /// @brief Systems which add bridges to the asset manager should be tagged with this.
    CUBOS_ENGINE_API extern Tag assetsBridgeTag;

    /// @brief Frees any assets no longer in use.
    CUBOS_ENGINE_API extern Tag assetsCleanupTag;

    /// @brief Startup systems which load assets should be tagged with this.
    CUBOS_ENGINE_API extern Tag assetsTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup assets-plugin
    CUBOS_ENGINE_API void assetsPlugin(Cubos& cubos);
} // namespace cubos::engine
