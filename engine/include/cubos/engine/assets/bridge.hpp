#pragma once

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    class Assets;

    /// Bridges are the objects responsible for loading and saving assets from/to disk.
    /// They form the bridge between the asset manager and the virtual file system.
    ///
    /// @details Not all bridges need to implement the save method. You could have an asset type
    /// which could be loaded but not saved, for example.
    class AssetBridge
    {
    public:
        AssetBridge() = default;
        virtual ~AssetBridge() = default;

        /// Loads an asset into the given asset manager, identified by the given handle.
        /// When called, the meta data of the given asset should be already present in the asset
        /// manager.
        /// @returns Whether the asset was successfully loaded.
        virtual bool load(Assets& assets, Asset handle) = 0;

        /// Saves an asset from the given asset manager, identified by the given handle.
        /// @returns Whether the asset was successfully saved.
        virtual bool save(Assets& assets, Asset handle);
    };
} // namespace cubos::engine
