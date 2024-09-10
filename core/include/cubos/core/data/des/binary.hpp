/// @file
/// @brief Class @ref cubos::core::data::BinaryDeserializer.
/// @ingroup core-data-des

#pragma once

#include <cubos/core/data/des/deserializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// @brief Implementation of the abstract Deserializer class meant to deserialize data written by a @ref
    /// BinarySerializer.
    class CUBOS_CORE_API BinaryDeserializer : public Deserializer
    {
    public:
        /// @brief Constructs.
        /// @param stream Stream to read from.
        BinaryDeserializer(memory::Stream& stream);

    protected:
        bool decompose(const reflection::Type& type, void* value) override;

    private:
        memory::Stream& mStream; ///< Stream to deserialize from.
    };
} // namespace cubos::core::data