/// @file
/// @brief Class @ref cubos::engine::FileBridge.
/// @ingroup assets-plugin

#pragma once

#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/assets.hpp>

namespace cubos::engine
{
    /// @brief Abstract bridge type defined to reduce boilerplate code in bridge implementations
    /// which open a single file to load and save assets.
    ///
    /// This bridge should be used as a base class for bridges which load and save assets. Child
    /// classes must implement the @ref loadFromFile() and @ref saveToFile() methods, which are
    /// called with a file stream to load and save the asset from and to, respectively.
    ///
    /// @ingroup assets-plugin
    class FileBridge : public AssetBridge
    {
    public:
        /// @brief Constructs a bridge.
        ///
        /// @param index Type of assets loaded by the bridge.
        explicit FileBridge(std::type_index index)
            : AssetBridge(index)
        {
        }

        bool load(Assets& assets, const AnyAsset& handle) final;
        bool save(const Assets& assets, const AnyAsset& handle) final;

    protected:
        /// @brief Loads an asset from a file stream.
        /// @param assets Manager to write into.
        /// @param handle Handle of the asset being loaded.
        /// @param stream File stream.
        /// @return Whether the asset was successfully loaded.
        virtual bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) = 0;

        /// @brief Saves an asset to a file stream.
        /// @param assets Manager to read from.
        /// @param handle Handle of the asset being saved.
        /// @param stream File stream.
        /// @return Whether the asset was successfully saved.
        virtual bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) = 0;
    };
} // namespace cubos::engine
