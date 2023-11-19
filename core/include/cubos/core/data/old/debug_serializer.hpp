#pragma once

#include <stack>

#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

namespace cubos::core::data::old
{
    /// Implementation of the abstract Serializer class for debugging purposes.
    /// This class is used internally by the logging functions.
    class DebugSerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        /// @param pretty Whether to pretty-print the output.
        /// @param typeNames Whether to print the type names.
        DebugSerializer(memory::Stream& stream, bool pretty = false, bool typeNames = false);
        ~DebugSerializer() override = default;

        // Implement interface methods.

        void writeI8(int8_t value, const char* name) override;
        void writeI16(int16_t value, const char* name) override;
        void writeI32(int32_t value, const char* name) override;
        void writeI64(int64_t value, const char* name) override;
        void writeU8(uint8_t value, const char* name) override;
        void writeU16(uint16_t value, const char* name) override;
        void writeU32(uint32_t value, const char* name) override;
        void writeU64(uint64_t value, const char* name) override;
        void writeF32(float value, const char* name) override;
        void writeF64(double value, const char* name) override;
        void writeBool(bool value, const char* name) override;
        void writeString(const char* value, const char* name) override;
        void beginObject(const char* name) override;
        void endObject() override;
        void beginArray(std::size_t length, const char* name) override;
        void endArray() override;
        void beginDictionary(std::size_t length, const char* name) override;
        void endDictionary() override;

    private:
        /// The possible state modes of serialization.
        enum class Mode
        {
            Object,
            Array,
            Dictionary
        };

        /// Data of a serialization state.
        struct State
        {
            Mode mode;    ///< The mode of the state.
            bool isKey;   ///< Whether the next write is a key or a value (if we are in a dictionary).
            bool isFirst; ///< Whether this is the first write in the state.
        };

        /// Prints spaces to indent the output.
        void printIndent();

        memory::Stream& mStream;  ///< The stream to serialize to.
        std::stack<State> mState; ///< The current state of the serializer.
        int mIndent;              ///< The current indentation level.
        bool mPretty;             ///< Whether to pretty-print the output.
        bool mTypeNames;          ///< Whether to print the type names.
    };
} // namespace cubos::core::data::old
