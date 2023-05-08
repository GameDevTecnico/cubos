#pragma once

#include <cubos/core/data/binary_deserializer.hpp>
#include <cubos/core/data/binary_serializer.hpp>

#include <cubos/engine/assets/assets.hpp>

namespace cubos::engine
{
    /// An abstract bridge type defined to reduce boilerplate code in bridge implementations which
    /// use binary (de)serialization to load and save assets.
    class BinaryBridge : public AssetBridge
    {
    public:
        bool load(Assets& assets, const AnyAsset& handle) override;
        bool save(const Assets& assets, const AnyAsset& handle) override;

    protected:
        /// Called after the file has been opened and the deserializer has been created.
        /// Errors can be reported by calling `deserializer.fail()`.
        /// @param assets The assets manager.
        /// @param handle The asset handle.
        /// @param deserializer The deserializer to read the asset from.
        virtual void loadImpl(Assets& assets, const AnyAsset& handle, core::data::BinaryDeserializer& deserializer) = 0;

        /// Called after the file has been opened and the serializer has been created.
        /// @param assets The assets manager.
        /// @param handle The asset handle.
        /// @param serializer The serializer to write the asset to.
        virtual void saveImpl(const Assets& assets, const AnyAsset& handle,
                              core::data::BinarySerializer& serializer) = 0;
    };

    /// Automatic implementation of BinaryBridge using a templated type.
    /// The type must be default constructible, serializable and deserializable.
    /// No extra context is passed to the serializer or deserializer, so this is only useful for
    /// simple types which do not require any context.
    /// @tparam T The type of asset to load and save.
    template <typename T>
    class BinaryBridgeImpl : public BinaryBridge
    {
    protected:
        void loadImpl(Assets& assets, const AnyAsset& handle, core::data::BinaryDeserializer& deserializer) override
        {
            T data{};
            deserializer.read(data);
            if (!deserializer.failed())
            {
                // Write the data to the asset only on success.
                assets.store(handle, std::move(data));
            }
        }

        void saveImpl(const Assets& assets, const AnyAsset& handle, core::data::BinarySerializer& serializer) override
        {
            // Read the data from the asset.
            auto data = assets.read<T>(handle);
            serializer.write(data, nullptr); // Binary serializer does not use names.
        }
    };
} // namespace cubos::engine
