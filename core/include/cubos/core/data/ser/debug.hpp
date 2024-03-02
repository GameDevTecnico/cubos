/// @file
/// @brief Class @ref cubos::core::data::DebugSerializer.
/// @ingroup core-data-ser

#pragma once

#include <cubos/core/data/ser/serializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// @brief Serializer implementation which prints the given data to a stream in a human-readable
    /// format not meant to be parsed.
    /// @ingroup core-data-ser
    class CUBOS_CORE_API DebugSerializer : public Serializer
    {
    public:
        /// @brief Constructs.
        /// @param stream Stream to serialize to.
        DebugSerializer(memory::Stream& stream);

        /// @brief Sets whether to pretty-print the output.
        /// @param pretty Whether to pretty-print the output.
        void pretty(bool pretty);

    protected:
        bool decompose(const reflection::Type& type, const void* value) override;

    private:
        void separate(bool first);

        memory::Stream& mStream; ///< Stream to serialize to.
        bool mPretty{false};     ///< Whether to pretty-print the output.
        int mLevel{0};           ///< Indentation level.
    };
} // namespace cubos::core::data
