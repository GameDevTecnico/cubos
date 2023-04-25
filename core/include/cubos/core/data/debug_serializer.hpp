#ifndef CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP
#define CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP

#include <stack>

#include <fmt/format.h>

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

namespace cubos::core::data
{
    /// Used to wrap a value to be printed using the debug serializer.
    /// Can be pretty-printed using the `p` option. Types can be printed using the `t` option.
    ///
    /// @details Example usage:
    ///
    ///    CUBOS_ERROR("Cool debug serialized object: {}", Debug(mySerializableObj));
    ///    CUBOS_INFO("Pretty printed: {:p}", Debug(mySerializableObj));
    ///    CUBOS_INFO("With types too: {:pt}", Debug(mySerializableObj));
    ///
    /// @tparam T The type of the value to wrap.
    template <typename T>
    struct Debug
    {
        /// @param value The value to debug.
        inline Debug(const T& value)
            : value(value)
        {
        }

        const T& value;
    };

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
        virtual void beginArray(std::size_t length, const char* name) override;
        virtual void endArray() override;
        virtual void beginDictionary(std::size_t length, const char* name) override;
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

// Add a formatter for Debug<T>.

/// @cond
template <typename T>
struct fmt::formatter<cubos::core::data::Debug<T>> : formatter<string_view>
{
    bool pretty = false; ///< Whether to pretty print the data.
    bool types = false;  ///< Whether to print the type name.

    inline constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        while (it != end)
        {
            if (*it == 'p')
                this->pretty = true;
            else if (*it == 't')
                this->types = true;
            else if (*it != '}')
                throw format_error("invalid format");
            else
                break;
            ++it;
        }
        return it;
    }

    template <typename FormatContext>
    inline auto format(const cubos::core::data::Debug<T>& dbg, FormatContext& ctx) -> decltype(ctx.out())
    {
        auto stream = cubos::core::memory::BufferStream(32);
        cubos::core::data::DebugSerializer serializer(stream, this->pretty, this->types);
        serializer.write(dbg.value, nullptr);
        stream.put('\0');
        // Skip the '?: ' prefix.
        auto result = std::string(static_cast<const char*>(stream.getBuffer()) + 3);
        return formatter<string_view>::format(string_view(result), ctx);
    }
};
/// @endcond

#endif // CUBOS_CORE_DATA_DEBUG_SERIALIZER_HPP
