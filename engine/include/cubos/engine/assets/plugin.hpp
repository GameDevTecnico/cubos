#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// Plugin which adds asset management to CUBOS.
    ///
    /// @details
    ///
    /// Events:
    /// - `AssetEvent`: emitted when an asset is either loaded, modified or unloaded.
    ///
    /// Resources:
    /// - `Assets`: the asset manager, used to access asset data.
    ///
    /// Startup tags:
    /// - `cubos.assets.meta`: loads the meta files for all assets (after `cubos.settings`).
    ///
    /// @param cubos CUBOS. main class.
    void assetsPlugin(Cubos& cubos);
} // namespace cubos::engine
