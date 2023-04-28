#include <cassert>

#include <cubos/core/data/yaml_serializer.hpp>

constexpr const char* ANONYMOUS_FIELD_NAME = "anonymous";

using namespace cubos::core::data;

YAMLSerializer::YAMLSerializer(memory::Stream& stream)
    : mStream(stream)
{
    this->mode.push(Mode::Object);
    this->key = true;
    mHead = 0;
    this->emitter << YAML::BeginDoc << YAML::BeginMap;
}

YAMLSerializer::~YAMLSerializer()
{
    this->emitter << YAML::EndMap << YAML::EndDoc;
    YAMLSerializer::flush();
}

void YAMLSerializer::flush()
{
    mStream.print(this->emitter.c_str() + mHead);
    mHead = this->emitter.size();
}

// Macro used to reduce the amount of code duplication.
// I decided to use this because every primitive write method has exactly the same code.
#define WRITE_PRIMITIVE(value, name)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mode.top() == Mode::Dictionary)                                                                            \
        {                                                                                                              \
            emitter << (this->key ? YAML::Key : YAML::Value) << (value);                                               \
            this->key = !this->key;                                                                                    \
        }                                                                                                              \
        else if (mode.top() == Mode::Array)                                                                            \
            emitter << (value);                                                                                        \
        else if (mode.top() == Mode::Object)                                                                           \
            emitter << YAML::Key << ((name) ? (name) : ANONYMOUS_FIELD_NAME) << YAML::Value << (value);                \
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
    if (mode.top() == Mode::Object)
    {
        emitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mode.top() == Mode::Dictionary)
    {
        assert(this->key == false); // Objects cannot be used as keys in dictionaries.
        emitter << YAML::Value;
    }

    emitter << YAML::BeginMap;
    this->mode.push(Mode::Object);
}

void YAMLSerializer::endObject()
{
    assert(this->mode.size() > 1 && this->mode.top() == Mode::Object);
    emitter << YAML::EndMap;
    this->mode.pop();
    if (this->mode.top() == Mode::Dictionary)
    {
        this->key = true;
    }
}

void YAMLSerializer::beginArray(std::size_t /*length*/, const char* name)
{
    if (mode.top() == Mode::Object)
    {
        emitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mode.top() == Mode::Dictionary)
    {
        assert(this->key == false); // Arrays cannot be used as keys in dictionaries.
        emitter << YAML::Value;
    }

    emitter << YAML::BeginSeq;
    this->mode.push(Mode::Array);
}

void YAMLSerializer::endArray()
{
    assert(this->mode.size() > 1 && this->mode.top() == Mode::Array);
    emitter << YAML::EndSeq;
    this->mode.pop();
    if (this->mode.top() == Mode::Dictionary)
    {
        this->key = true;
    }
}

void YAMLSerializer::beginDictionary(std::size_t /*length*/, const char* name)
{
    if (mode.top() == Mode::Object)
    {
        emitter << YAML::Key << (name != nullptr ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
    }
    else if (mode.top() == Mode::Dictionary)
    {
        assert(this->key == false); // Dictionaries cannot be used as keys in dictionaries.
        emitter << YAML::Value;
    }

    emitter << YAML::BeginMap;
    this->mode.push(Mode::Dictionary);
}

void YAMLSerializer::endDictionary()
{
    assert(this->mode.size() > 1 && this->mode.top() == Mode::Dictionary);
    emitter << YAML::EndMap;
    this->mode.pop();
    if (this->mode.top() == Mode::Dictionary)
    {
        this->key = true;
    }
}
