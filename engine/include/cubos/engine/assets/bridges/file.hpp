#pragma once

#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/assets.hpp>

namespace cubos::engine
{
    /// An abstract bridge type defined to reduce boilerplate code in bridge implementations which
    /// open a single file to load and save assets.
    ///
    /// @details This bridge should be used as a base class for bridges which load and save assets.
    /// Child classes must implement the `loadFromFile` and `saveToFile` methods, which are called
    /// with a file stream to load and save the asset from and to, respectively.
    class FileBridge : public AssetBridge
    {
    public:
        bool load(Assets& assets, const AnyAsset& handle) final;
        bool save(const Assets& assets, const AnyAsset& handle) final;

    protected:
        /// Called with the file stream to load the asset from.
        virtual bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) = 0;

        /// Called with the file stream to save the asset to.
        virtual bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) = 0;
    };
} // namespace cubos::engine
