/// @file
/// @brief Defines the DebugSerializer class.

#pragma once

#include <cubos/core/data/ser/serializer.hpp>

namespace cubos::core::memory
{
    class Stream;
}

namespace cubos::core::data
{
    /// @brief Serializer implementation that prints the serialized data to a stream in a human
    /// readable format, not intended for machine parsing.
    ///
    /// @details By default, this serializer sets hooks for booleans, chars, integer types,
    /// floating-point types and for std::string. Hooks for other primitive types must be set
    /// manually.
    class DebugSerializer : public Serializer
    {
    public:
        /// @brief Construct a new DebugSerializer object.
        /// @param stream Stream to write to.
        /// @param pretty Whether to indent the output.
        /// @param typeNames Whether to print type names.
        DebugSerializer(memory::Stream& stream, bool pretty, bool typeNames);

    protected:
        void writeObject(const reflection::ObjectType& type, const void* data) override;
        void writeVariant(const reflection::VariantType& type, const void* data) override;
        void writeArray(const reflection::ArrayType& type, const void* data) override;
        void writeDictionary(const reflection::DictionaryType& type, const void* data) override;

    private:
        void separate();
        void typeName(const reflection::Type& type, bool shortened);

        memory::Stream& mStream;
        bool mPretty;
        bool mTypeNames;
        int mIndent;
    };
} // namespace cubos::core::data
