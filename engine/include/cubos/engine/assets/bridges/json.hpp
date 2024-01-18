/// @file
/// @brief Class @ref cubos::engine::JSONBridge.
/// @ingroup assets-plugin

#pragma once

#include <cubos/core/data/des/json.hpp>
#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
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
        JSONBridge()
            : FileBridge(core::reflection::reflect<T>())
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override
        {
            // Dump the file stream into a string and initialize a JSON deserializer with it.
            std::string jsonStr;
            stream.readUntil(jsonStr, nullptr);
            core::data::JSONDeserializer deserializer{};

            // JSONDeserializer() receives a JSON object to deserialize from
            nlohmann::json json{};

            try
            {
                json = nlohmann::json::parse(jsonStr);
            }
            catch (nlohmann::json::parse_error& e)
            {
                CUBOS_ERROR("{}", e.what());
                return false;
            }
            deserializer.feed(json);

            // Deserialize the asset and store it in the asset manager.
            T data{};
            if (deserializer.read(data))
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
            core::data::JSONSerializer serializer{};

            // Read the asset from the asset manager and serialize it to the file stream.
            auto data = assets.read<T>(handle);
            if (serializer.write(*data))
            {
                CUBOS_ERROR("Could not serialize asset to JSON file");
                return false;
            }

            // new JSONSerializer() does not receive a stream to write to, need to write to it manually
            auto jsonStr = serializer.output().dump();
            stream.print(jsonStr);
            return true;
        }
    };
} // namespace cubos::engine
