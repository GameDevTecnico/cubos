/// @file
/// @brief Class @ref cubos::core::data::JSONSerializer.
/// @ingroup core-data-ser

#pragma once

#include <nlohmann/json.hpp>

#include <cubos/core/data/ser/serializer.hpp>

namespace cubos::core::data
{
    /// @brief Implementation of the abstract Serializer class for serializing to JSON.
    class JSONSerializer : public Serializer
    {
    public:
        /// @brief Constructs.
        JSONSerializer();

        /// @brief Outputs the serialized JSON.
        nlohmann::json output();

    protected:
        bool decompose(const reflection::Type& type, const void* value) override;

    private:
        nlohmann::json mJSON = nlohmann::json::object(); ///< The JSON object that holds the serialized data.
    };
} // namespace cubos::core::data