/// @file
/// @brief Class @ref cubos::core::data::JSONDeserializer.
/// @ingroup core-data-des

#pragma once

#include <nlohmann/json.hpp>

#include <cubos/core/data/des/deserializer.hpp>

namespace cubos::core::data
{
    /// @brief Deserializer implementation which allows reading data from a JSON object.
    ///
    /// Before deserializing any data, a JSON object must be fed to the deserializer.
    ///
    /// @ingroup core-data-des
    class CUBOS_CORE_API JSONDeserializer : public Deserializer
    {
    public:
        /// @brief Constructs.
        JSONDeserializer();

        /// @brief Feeds a JSON object to be deserialized.
        /// @param json JSON object.
        void feed(nlohmann::json json);

    protected:
        bool decompose(const reflection::Type& type, void* value) override;

    private:
        nlohmann::json mJSON;
        nlohmann::json::iterator mIterator;
        bool mIsKey;
    };
} // namespace cubos::core::data
