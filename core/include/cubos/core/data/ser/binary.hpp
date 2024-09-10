/// @file
/// @brief Class @ref cubos::core::data::BinarySerializer.
/// @ingroup core-data-ser

#pragma once

#include <cubos/core/data/ser/serializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// @brief Implementation of the abstract Serializer class meant to serialize data in a non-readable but fast and
    /// compact way.
    class CUBOS_CORE_API BinarySerializer : public Serializer
    {
    public:
        /// @brief Constructs.
        /// @param stream Stream to write to.
        BinarySerializer(memory::Stream& stream);

    protected:
        bool decompose(const reflection::Type& type, const void* value) override;

    private:
        memory::Stream& mStream; ///< Stream to serialize to.
    };
} // namespace cubos::core::data