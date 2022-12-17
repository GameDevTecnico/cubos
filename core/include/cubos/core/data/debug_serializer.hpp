#ifndef CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP
#define CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP

#include <cubos/core/data/serializer.hpp>

#include <stack>

namespace cubos::core::data
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
        virtual ~DebugSerializer() override = default;

        // Implement interface methods.

        virtual void writeI8(int8_t value, const char* name) override;
        virtual void writeI16(int16_t value, const char* name) override;
        virtual void writeI32(int32_t value, const char* name) override;
        virtual void writeI64(int64_t value, const char* name) override;
        virtual void writeU8(uint8_t value, const char* name) override;
        virtual void writeU16(uint16_t value, const char* name) override;
        virtual void writeU32(uint32_t value, const char* name) override;
        virtual void writeU64(uint64_t value, const char* name) override;
        virtual void writeF32(float value, const char* name) override;
        virtual void writeF64(double value, const char* name) override;
        virtual void writeBool(bool value, const char* name) override;
        virtual void writeString(const char* value, const char* name) override;
        virtual void beginObject(const char* name) override;
        virtual void endObject() override;
        virtual void beginArray(size_t length, const char* name) override;
        virtual void endArray() override;
        virtual void beginDictionary(size_t length, const char* name) override;
        virtual void endDictionary() override;

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

        memory::Stream& stream;  ///< The stream to serialize to.
        std::stack<State> state; ///< The current state of the serializer.
        int indent;              ///< The current indentation level.
        bool pretty;             ///< Whether to pretty-print the output.
        bool typeNames;          ///< Whether to print the type names.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP
