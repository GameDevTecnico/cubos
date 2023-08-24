/// @file
/// @brief Class @ref cubos::engine::AssetBridge.
/// @ingroup assets-plugin

/// @dir ./bridges
/// @brief @ref cubos::engine::AssetBridge implementations directory.

#pragma once

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    class Assets;

    /// @brief Bridges are the objects responsible for loading and saving assets from/to disk.
    /// They form the bridge between the asset manager and the virtual file system.
    ///
    /// Not all bridges need to implement the save method. You could have an asset type
    /// which could be loaded but not saved, for example.
    ///
    /// Bridges should take into account that the asset manager calls them from a different thread
    /// than the one that created them.
    ///
    /// @ingroup assets-plugin
    class AssetBridge
    {
    public:
        AssetBridge() = default;
        virtual ~AssetBridge() = default;

        /// @brief Loads an asset.
        ///
        /// The metadata of the given asset should already be present in the asset manager.
        ///
        /// @param assets Manager to write into.
        /// @param handle Handle of the asset being loaded.
        /// @return Whether the asset was successfully loaded.
        virtual bool load(Assets& assets, const AnyAsset& handle) = 0;

        /// @brief Saves an asset.
        ///
        /// The asset should be loaded in the asset manager.
        ///
        /// @param assets Manager to read from.
        /// @param handle Handle of the asset being saved.
        /// @return Whether the asset was successfully saved.
        virtual bool save(const Assets& assets, const AnyAsset& handle);
    };
} // namespace cubos::engine
