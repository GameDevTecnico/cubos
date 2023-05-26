#pragma once

#include <cubos/core/data/binary_deserializer.hpp>
#include <cubos/core/data/binary_serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// A bridge for loading and saving assets which are serialized to and from a binary file.
    ///
    /// @details This bridge automatically serializes and deserializes assets of type `T` to and
    /// from a binary file. Thus, `T` must be serializable and deserializable. No additional
    /// context is given to the serializer or deserializer.
    ///
    /// @tparam T The type of asset to load and save. Must be default constructible.
    template <typename T>
    class BinaryBridge : public FileBridge
    {
    public:
        /// @param littleEndian Whether to use little endian byte order.
        BinaryBridge(bool littleEndian = true)
            : mLittleEndian{littleEndian}
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Initialize a binary deserializer with the file stream.
            core::data::BinaryDeserializer deserializer{stream, mLittleEndian};

            // Deserialize the asset and store it in the asset manager.
            T data{};
            deserializer.read(data);
            if (deserializer.failed())
            {
                CUBOS_ERROR("Could not deserialize binary file '{}'", path);
                return false;
            }

            assets.store(handle, std::move(data));
            return true;
        }

        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Initialize a binary serializer with the file stream.
            core::data::BinarySerializer serializer{*stream, mLittleEndian};

            // Read the asset from the asset manager and serialize it to the file stream.
            auto data = assets.read<T>(handle);
            serializer.write(*data, nullptr); // Binary serializer does not use names.
            if (serializer.failed())
            {
                CUBOS_ERROR("Could not serialize asset to binary file '{}'", path);
                return false;
            }

            return true;
        }

    private:
        bool mLittleEndian;
    };
} // namespace cubos::engine
