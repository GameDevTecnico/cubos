#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

DebugSerializer::DebugSerializer(memory::Stream& stream, bool pretty, bool typeNames)
    : mStream(stream)
{
    mIndent = 0;
    mPretty = pretty;
    mTypeNames = typeNames;
    mState.push(State{Mode::Object, false, true});
}

void DebugSerializer::printIndent()
{
    for (int i = 0; i < mIndent * 2; ++i)
    {
        mStream.put(' ');
    }
}

// Prints a generic value.
#define GENERIC_WRITE(...)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        /* Print the separator. */                                                                                     \
        auto& state = mState.top();                                                                               \
        if (state.isFirst)                                                                                             \
        {                                                                                                              \
            state.isFirst = false;                                                                                     \
            if (mPretty)                                                                                          \
            {                                                                                                          \
                mStream.put('\n');                                                                                \
                this->printIndent();                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
        else if (state.mode == Mode::Dictionary && !state.isKey)                                                       \
            mStream.print(": ");                                                                                  \
        else if (mPretty)                                                                                         \
        {                                                                                                              \
            mStream.print(",\n");                                                                                 \
            this->printIndent();                                                                                       \
        }                                                                                                              \
        else                                                                                                           \
            mStream.print(", ");                                                                                  \
                                                                                                                       \
        /* Print the name of the value, if its an object. */                                                           \
        if (state.mode == Mode::Object)                                                                                \
            mStream.printf("{}: ", name == nullptr ? "?" : name);                                                 \
                                                                                                                       \
        /* Print the value, with its type name, if enabled. */                                                         \
        mStream.printf(__VA_ARGS__);                                                                              \
                                                                                                                       \
        /* If it's a dictionary, flip the isKey flag. */                                                               \
        if (state.mode == Mode::Dictionary)                                                                            \
            state.isKey = !state.isKey;                                                                                \
    } while (false)

void DebugSerializer::writeI8(int8_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "i8" : "");
}

void DebugSerializer::writeI16(int16_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "i16" : "");
}

void DebugSerializer::writeI32(int32_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "i32" : "");
}

void DebugSerializer::writeI64(int64_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "i64" : "");
}

void DebugSerializer::writeU8(uint8_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "u8" : "");
}

void DebugSerializer::writeU16(uint16_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "u16" : "");
}

void DebugSerializer::writeU32(uint32_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "u32" : "");
}

void DebugSerializer::writeU64(uint64_t value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "u64" : "");
}

void DebugSerializer::writeF32(float value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "f32" : "");
}

void DebugSerializer::writeF64(double value, const char* name)
{
    GENERIC_WRITE("{}{}", value, mTypeNames ? "f64" : "");
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
    mState.push({Mode::Object, false, true});
    mIndent++;
}

void DebugSerializer::endObject()
{
    mIndent--;
    mState.pop();
    if (mPretty)
    {
        mStream.put('\n');
        this->printIndent();
    }
    mStream.put(')');
}

void DebugSerializer::beginArray(std::size_t length, const char* name)
{
    if (mTypeNames)
    {
        GENERIC_WRITE("{}x[", length);
    }
    else
    {
        GENERIC_WRITE("[");
    }
    mState.push({Mode::Array, false, true});
    mIndent++;
}

void DebugSerializer::endArray()
{
    mIndent--;
    mState.pop();
    if (mPretty)
    {
        mStream.put('\n');
        this->printIndent();
    }
    mStream.put(']');
}

void DebugSerializer::beginDictionary(std::size_t length, const char* name)
{
    if (mTypeNames)
    {
        GENERIC_WRITE("{}x{", length);
    }
    else
    {
        GENERIC_WRITE("{");
    }
    mState.push({Mode::Dictionary, true, true});
    mIndent++;
}

void DebugSerializer::endDictionary()
{
    mIndent--;
    mState.pop();
    if (mPretty)
    {
        mStream.put('\n');
        this->printIndent();
    }
    mStream.put('}');
}
