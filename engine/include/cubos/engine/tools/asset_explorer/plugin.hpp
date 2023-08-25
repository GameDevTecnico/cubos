/// @dir
/// @brief @ref asset-explorer-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup asset-explorer-tool-plugin

#pragma once

#include <cubos/engine/assets/plugin.hpp>

namespace cubos::engine::tools
{
    /// @defgroup asset-explorer-tool-plugin Asset explorer
    /// @ingroup tool-plugins
    /// @brief Allows viewing and selecting assets through a ImGui window.
    ///
    /// ## Events
    /// - @ref AssetSelectedEvent - sent when an asset is selected in the explorer.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref assets-plugin

    /// @brief Event sent when an asset is selected.
    struct AssetSelectedEvent
    {
        AnyAsset asset; ///< Handle to the selected asset.
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup asset-explorer-tool-plugin
    void assetExplorerPlugin(Cubos& cubos);
} // namespace cubos::engine::tools