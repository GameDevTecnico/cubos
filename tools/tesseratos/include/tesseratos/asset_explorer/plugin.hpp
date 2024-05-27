/// @dir
/// @brief @ref tesseratos-asset-explorer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-asset-explorer-plugin

#pragma once

#include <cubos/engine/assets/plugin.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-asset-explorer-plugin Asset explorer
    /// @ingroup tesseratos
    /// @brief Allows viewing and selecting assets through a ImGui window.
    ///
    /// ## Events
    /// - @ref AssetSelectedEvent - sent when an asset is selected in the explorer.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref assets-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Event sent when an asset is selected.
    struct AssetSelectedEvent
    {
        cubos::engine::AnyAsset asset; ///< Handle to the selected asset.
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-asset-explorer-plugin
    void assetExplorerPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
