#pragma once

#include <cubos/core/data/json_deserializer.hpp>
#include <cubos/core/data/json_serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// A bridge for loading and saving assets which are serialized to and from a JSON file.
    ///
    /// @details This bridge automatically serializes and deserializes assets of type `T` to and
    /// from a JSON file. Thus, `T` must be serializable and deserializable. No additional context
    /// is given to the serializer or deserializer.
    ///
    /// @tparam T The type of asset to load and save. Must be default constructible.
    template <typename T>
    class JSONBridge : public FileBridge
    {
    public:
        /// @param indentation The indentation level to use when saving the JSON file. Set to -1 to
        /// remove all whitespace.
        JSONBridge(int indentation = 4)
            : mIndentation{indentation}
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Dump the file stream into a string and initialize a JSON deserializer with it.
            std::string json{};
            stream.readUntil(json, nullptr);
            core::data::JSONDeserializer deserializer{json};

            // Deserialize the asset and store it in the asset manager.
            T data{};
            deserializer.read(data);
            if (deserializer.failed())
            {
                CUBOS_ERROR("Could not deserialize asset from JSON file");
                return false;
            }

            assets.store(handle, std::move(data));
            return true;
        }

        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Initialize a JSON serializer with the file stream.
            core::data::JSONSerializer serializer{stream, mIndentation};

            // Read the asset from the asset manager and serialize it to the file stream.
            auto data = assets.read<T>(handle);
            serializer.write(*data, nullptr); // JSON serializer does not use names.
            if (serializer.failed())
            {
                CUBOS_ERROR("Could not serialize asset to JSON file");
                return false;
            }

            return true;
        }

    private:
        int mIndentation;
    };
} // namespace cubos::engine
