/// @file
/// @brief Defines the JSONDeserializer class.

#pragma once

#include <nlohmann/json.hpp>

#include <cubos/core/data/des/deserializer.hpp>

namespace cubos::core::data
{
    /// @brief Deserializer implementation which reads data from a JSON string.
    ///
    /// @details This deserializer uses the nlohmann::json library to parse JSON strings.
    /// Non-primitive types cannot be used as keys in dictionaries, since JSON keys must be strings.
    class JSONDeserializer : public Deserializer
    {
    public:
        JSONDeserializer();

        /// @brief Parses a JSON string and stores the result for deserialization.
        /// @param json JSON to be parsed.
        /// @return Whether the JSON was parsed successfully.
        bool parse(std::string&& json);

        /// @brief Sets the JSON to be deserialized.
        /// @param json JSON to be deserialized.
        void feed(nlohmann::json&& json);

        /// @brief Sets whether the deserializer should allow objects to be missing ifelds.
        /// @param allow Whether to allow missing fields.
        void allowsMissingFields(bool allow);

    protected:
        bool readKey(const std::string& key, const reflection::Type& type, void* data);
        bool readValue(const nlohmann::json* json, const reflection::Type& type, void* data);

        bool readObject(const reflection::ObjectType& type, void* data) override;
        bool readVariant(const reflection::VariantType& type, void* data) override;
        bool readArray(const reflection::ArrayType& type, void* data) override;
        bool readDictionary(const reflection::DictionaryType& type, void* data) override;

        nlohmann::json mJSON;
        bool mAllowsMissingFields;

        /// @brief Pointer to the current JSON key being deserialized, if any.
        const std::string* mKey;

        /// @brief Pointer to the current JSON value being deserialized.
        const nlohmann::json* mValue;
    };
} // namespace cubos::core::data
