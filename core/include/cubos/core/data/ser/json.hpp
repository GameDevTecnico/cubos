/// @file
/// @brief Defines the JSONSerializer class.

#pragma once

#include <nlohmann/json.hpp>

#include <cubos/core/data/ser/serializer.hpp>

namespace cubos::core::data
{
    /// @brief Serializer implementation which outputs JSON data.
    ///
    /// @details This serializer uses the nlohmann::json library to manage JSON data.
    /// Non-primitive types cannot be used as keys in dictionaries, since JSON keys must be strings.
    class JSONSerializer : public Serializer
    {
    public:
        JSONSerializer();

        /// @brief Extracts the current JSON data and clears it.
        /// @return The current JSON data.
        nlohmann::json json();

        /// @brief Extracts the current JSON data as a string and clears it.
        /// @param indent Number of spaces to indent the JSON (or -1 for no newlines).
        /// @return The current JSON data as a string.
        std::string string(int indent = -1);

    protected:
        void writeKey(std::string& key, const reflection::Type& type, const void* data);
        void writeValue(nlohmann::json& json, const reflection::Type& type, const void* data);

        void writeObject(const reflection::ObjectType& type, const void* data) override;
        void writeVariant(const reflection::VariantType& type, const void* data) override;
        void writeArray(const reflection::ArrayType& type, const void* data) override;
        void writeDictionary(const reflection::DictionaryType& type, const void* data) override;

        nlohmann::json mJSON;

        /// @brief Pointer to the current JSON key being serialized, if any.
        std::string* mKey;

        /// @brief Pointer to the current JSON value being serialized.
        nlohmann::json* mValue;
    };
} // namespace cubos::core::data
