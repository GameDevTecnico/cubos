/// @file
/// @brief Class @ref cubos::core::data::JSONSerializer.
/// @ingroup core-data-ser

#pragma once

#include <nlohmann/json.hpp>

#include <cubos/core/data/ser/serializer.hpp>

namespace cubos::core::data
{
    /// @brief Implementation of the abstract Serializer class for serializing to JSON.
    class CUBOS_CORE_API JSONSerializer : public Serializer
    {
    public:
        /// @brief Constructs.
        JSONSerializer();

        /// @brief Outputs the serialized JSON.
        /// @ref write must be called before this.
        /// @return JSON.
        nlohmann::json output();

    protected:
        bool decompose(const reflection::Type& type, const void* value) override;

    private:
        /// @brief The resulting JSON object.
        /// @note It's also used to store the value of a variable on an AUTO_HOOK call.
        nlohmann::json mJSON = nlohmann::json::object();
    };
} // namespace cubos::core::data