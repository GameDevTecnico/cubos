#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

DebugSerializer::DebugSerializer(memory::Stream& stream, bool pretty, bool typeNames)
    : stream(stream)
{
    this->indent = 0;
    this->pretty = pretty;
    this->typeNames = typeNames;
    this->state.push(State{Mode::Object, false, true});
}

void DebugSerializer::printIndent()
{
    for (int i = 0; i < this->indent * 2; ++i)
    {
        this->stream.put(' ');
    }
}

// Prints a generic value.
#define GENERIC_WRITE(...)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        /* Print the separator. */                                                                                     \
        auto& state = this->state.top();                                                                               \
        if (state.isFirst)                                                                                             \
        {                                                                                                              \
            state.isFirst = false;                                                                                     \
            if (this->pretty)                                                                                          \
            {                                                                                                          \
                this->stream.put('\n');                                                                                \
                this->printIndent();                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
        else if (state.mode == Mode::Dictionary && !state.isKey)                                                       \
            this->stream.print(": ");                                                                                  \
        else if (this->pretty)                                                                                         \
        {                                                                                                              \
            this->stream.print(",\n");                                                                                 \
            this->printIndent();                                                                                       \
        }                                                                                                              \
        else                                                                                                           \
            this->stream.print(", ");                                                                                  \
                                                                                                                       \
        /* Print the name of the value, if its an object. */                                                           \
        if (state.mode == Mode::Object)                                                                                \
            this->stream.printf("{}: ", name == nullptr ? "?" : name);                                                 \
                                                                                                                       \
        /* Print the value, with its type name, if enabled. */                                                         \
        this->stream.printf(__VA_ARGS__);                                                                              \
                                                                                                                       \
        /* If it's a dictionary, flip the isKey flag. */                                                               \
        if (state.mode == Mode::Dictionary)                                                                            \
            state.isKey = !state.isKey;                                                                                \
    } while (false)

void DebugSerializer::writeI8(int8_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "i8" : "");
}

void DebugSerializer::writeI16(int16_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "i16" : "");
}

void DebugSerializer::writeI32(int32_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "i32" : "");
}

void DebugSerializer::writeI64(int64_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "i64" : "");
}

void DebugSerializer::writeU8(uint8_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "u8" : "");
}

void DebugSerializer::writeU16(uint16_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "u16" : "");
}

void DebugSerializer::writeU32(uint32_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "u32" : "");
}

void DebugSerializer::writeU64(uint64_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "u64" : "");
}

void DebugSerializer::writeF32(float value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "f32" : "");
}

void DebugSerializer::writeF64(double value, const char* name)
{
    GENERIC_WRITE("{}{}", value, this->typeNames ? "f64" : "");
}

void DebugSerializer::writeBool(bool value, const char* name)
{
    GENERIC_WRITE(value ? "true" : "false");
}

void DebugSerializer::writeString(const char* value, const char* name)
{
    GENERIC_WRITE("\"{}\"", value);
}

void DebugSerializer::beginObject(const char* name)
{
    GENERIC_WRITE("(");
    this->state.push({Mode::Object, false, true});
    this->indent++;
}

void DebugSerializer::endObject()
{
    this->indent--;
    this->state.pop();
    if (this->pretty)
    {
        this->stream.put('\n');
        this->printIndent();
    }
    this->stream.put(')');
}

void DebugSerializer::beginArray(size_t length, const char* name)
{
    if (this->typeNames)
    {
        GENERIC_WRITE("{}x[", length);
    }
    else
    {
        GENERIC_WRITE("[");
    }
    this->state.push({Mode::Array, false, true});
    this->indent++;
}

void DebugSerializer::endArray()
{
    this->indent--;
    this->state.pop();
    if (this->pretty)
    {
        this->stream.put('\n');
        this->printIndent();
    }
    this->stream.put(']');
}

void DebugSerializer::beginDictionary(size_t length, const char* name)
{
    if (this->typeNames)
    {
        GENERIC_WRITE("{}x{", length);
    }
    else
    {
        GENERIC_WRITE("{");
    }
    this->state.push({Mode::Dictionary, true, true});
    this->indent++;
}

void DebugSerializer::endDictionary()
{
    this->indent--;
    this->state.pop();
    if (this->pretty)
    {
        this->stream.put('\n');
        this->printIndent();
    }
    this->stream.put('}');
}
