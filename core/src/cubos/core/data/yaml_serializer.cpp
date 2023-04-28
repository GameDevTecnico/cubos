#include <cassert>

#include <cubos/core/data/yaml_serializer.hpp>

constexpr const char* ANONYMOUS_FIELD_NAME = "anonymous";

using namespace cubos::core::data;

YAMLSerializer::YAMLSerializer(memory::Stream& stream)
    : mStream(stream)
{
    mMode.push(Mode::Object);
    mKey = true;
    mHead = 0;
    mEmitter << YAML::BeginDoc << YAML::BeginMap;
}

YAMLSerializer::~YAMLSerializer()
{
    mEmitter << YAML::EndMap << YAML::EndDoc;
    YAMLSerializer::flush();
}

void YAMLSerializer::flush()
{
    mStream.print(mEmitter.c_str() + mHead);
    mHead = mEmitter.size();
}

// Macro used to reduce the amount of code duplication.
// I decided to use this because every primitive write method has exactly the same code.
#define WRITE_PRIMITIVE(value, name)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mMode.top() == Mode::Dictionary)                                                                           \
        {                                                                                                              \
            mEmitter << (mKey ? YAML::Key : YAML::Value) << (value);                                                   \
            mKey = !mKey;                                                                                              \
        }                                                                                                              \
        else if (mMode.top() == Mode::Array)                                                                           \
            mEmitter << (value);                                                                                       \
        else if (mMode.top() == Mode::Object)                                                                          \
            mEmitter << YAML::Key << ((name) ? (name) : ANONYMOUS_FIELD_NAME) << YAML::Value << (value);               \
    } while (false)

void YAMLSerializer::writeI8(int8_t value, const char* name)
{
    WRITE_PRIMITIVE(static_cast<int16_t>(value), name);
}

void YAMLSerializer::writeI16(int16_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeI32(int32_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeI64(int64_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeU8(uint8_t value, const char* name)
{
    WRITE_PRIMITIVE(static_cast<uint16_t>(value), name);
}

void YAMLSerializer::writeU16(uint16_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeU32(uint32_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeU64(uint64_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeF32(float value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeF64(double value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeBool(bool value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::writeString(const char* value, const char* name)
{
    assert(value != nullptr);
    WRITE_PRIMITIVE(value, name);
}

#undef WRITE_PRIMITIVE

void YAMLSerializer::beginObject(const char* name)
{
    if (mMode.top() == Mode::Object)
    {
        mEmitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mMode.top() == Mode::Dictionary)
    {
        assert(mKey == false); // Objects cannot be used as keys in dictionaries.
        mEmitter << YAML::Value;
    }

    mEmitter << YAML::BeginMap;
    mMode.push(Mode::Object);
}

void YAMLSerializer::endObject()
{
    assert(mMode.size() > 1 && mMode.top() == Mode::Object);
    mEmitter << YAML::EndMap;
    mMode.pop();
    if (mMode.top() == Mode::Dictionary)
    {
        mKey = true;
    }
}

void YAMLSerializer::beginArray(std::size_t /*length*/, const char* name)
{
    if (mMode.top() == Mode::Object)
    {
        mEmitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mMode.top() == Mode::Dictionary)
    {
        assert(mKey == false); // Arrays cannot be used as keys in dictionaries.
        mEmitter << YAML::Value;
    }

    mEmitter << YAML::BeginSeq;
    mMode.push(Mode::Array);
}

void YAMLSerializer::endArray()
{
    assert(mMode.size() > 1 && mMode.top() == Mode::Array);
    mEmitter << YAML::EndSeq;
    mMode.pop();
    if (mMode.top() == Mode::Dictionary)
    {
        mKey = true;
    }
}

void YAMLSerializer::beginDictionary(std::size_t /*length*/, const char* name)
{
    if (mMode.top() == Mode::Object)
    {
        mEmitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mMode.top() == Mode::Dictionary)
    {
        assert(mKey == false); // Dictionaries cannot be used as keys in dictionaries.
        mEmitter << YAML::Value;
    }

    mEmitter << YAML::BeginMap;
    mMode.push(Mode::Dictionary);
}

void YAMLSerializer::endDictionary()
{
    assert(mMode.size() > 1 && mMode.top() == Mode::Dictionary);
    mEmitter << YAML::EndMap;
    mMode.pop();
    if (mMode.top() == Mode::Dictionary)
    {
        mKey = true;
    }
}
