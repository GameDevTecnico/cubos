/// @file
/// @brief Class @ref cubos::engine::AssetBridge.
/// @ingroup assets-plugin

/// @dir ./bridges
/// @brief @ref cubos::engine::AssetBridge implementations directory.

#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

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
    class CUBOS_ENGINE_API AssetBridge
    {
    public:
        /// @brief Constructs a bridge.
        ///
        /// @param type Type of assets loaded by the bridge.
        explicit AssetBridge(const core::reflection::Type& type)
            : mType(type)
        {
        }

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

        /// @brief Gets the type of the assets the bridge loads.
        /// @return Type of the asset.
        inline const core::reflection::Type& assetType() const
        {
            return mType;
        }

    private:
        const core::reflection::Type& mType; ///< Type of assets loaded by the bridge
    };
} // namespace cubos::engine
