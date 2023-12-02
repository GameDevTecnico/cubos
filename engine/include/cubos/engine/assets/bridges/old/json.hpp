/// @file
/// @brief Class @ref cubos::engine::JSONBridge.
/// @ingroup assets-plugin

#pragma once

#include <cubos/core/data/old/json_deserializer.hpp>
#include <cubos/core/data/old/json_serializer.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine::old
{
    /// @brief Bridge for loading and saving assets which are serialized to and from a JSON file.
    ///
    /// This bridge automatically serializes and deserializes assets of type @p T to and from a
    /// JSON file. Thus, @p T must be serializable and deserializable. No additional context is
    /// given to the serializer or deserializer.
    ///
    /// @tparam T Type of asset to load and save. Must be default constructible.
    /// @ingroup assets-plugin
    template <typename T>
    class JSONBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        ///
        /// If the @p indentation level is set to -1, all whitespace is removed.
        ///
        /// @param indentation Indentation level to use when saving the JSON file.
        JSONBridge(int indentation = 4)
            : FileBridge(core::reflection::reflect<T>())
            , mIndentation{indentation}
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Dump the file stream into a string and initialize a JSON deserializer with it.
            std::string json{};
            stream.readUntil(json, nullptr);
            core::data::old::JSONDeserializer deserializer{json};

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
            core::data::old::JSONSerializer serializer{stream, mIndentation};

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
