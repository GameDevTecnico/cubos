#pragma once

#include <cubos/engine/assets/plugin.hpp>

namespace cubos::engine::tools
{
    /// @brief Event sent when an asset is selected.
    struct AssetSelectedEvent
    {
        AnyAsset asset;
    };

    /// Plugin that allows exploring and selecting assets through a ImGui window.
    ///
    /// @details This plugin adds one system, which adds a ImGui window with the assets folder.
    ///
    /// Dependencies:
    /// - `imguiPlugin`
    /// - `assetsPlugin`
    ///
    /// Events:
    /// - `AssetSelectedEvent`: sent when an asset is selected in the explorer.
    ///
    /// @param cubos CUBOS. main class
    void assetExplorerPlugin(Cubos& cubos);
} // namespace cubos::engine::tools