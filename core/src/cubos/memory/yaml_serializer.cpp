#include <cubos/memory/yaml_serializer.hpp>

constexpr const char* ANONYMOUS_FIELD_NAME = "anonymous";

using namespace cubos::memory;

YAMLSerializer::YAMLSerializer(Stream& stream) : Serializer(stream)
{
    this->mode.push(Mode::Object);
    this->key = true;
    this->head = 0;
    this->emitter << YAML::BeginDoc << YAML::BeginMap;
}

YAMLSerializer::~YAMLSerializer()
{
    this->emitter << YAML::EndMap << YAML::EndDoc;
    this->flush();
}

void YAMLSerializer::flush()
{
    this->stream.print(this->emitter.c_str() + this->head);
    this->head = this->emitter.size();
}

// Macro used to reduce the amount of code duplication.
// I decided to use this because every primitive write method has exactly the same code.
#define WRITE_PRIMITIVE(value, name)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mode.top() == Mode::Dictionary)                                                                            \
        {                                                                                                              \
            emitter << (this->key ? YAML::Key : YAML::Value) << value;                                                 \
            this->key = !this->key;                                                                                    \
        }                                                                                                              \
        else if (mode.top() == Mode::Array)                                                                            \
            emitter << value;                                                                                          \
        else if (mode.top() == Mode::Object)                                                                           \
            emitter << YAML::Key << (name ? name : ANONYMOUS_FIELD_NAME) << YAML::Value << value;                      \
    } while (false)

void YAMLSerializer::write(int8_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(int16_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(int32_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(int64_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(uint8_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(uint16_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(uint32_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(uint64_t value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(float value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(double value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(bool value, const char* name)
{
    WRITE_PRIMITIVE(value, name);
}

void YAMLSerializer::write(const char* value, const char* name)
{
    assert(value != nullptr);
    WRITE_PRIMITIVE(value, name);
}

#undef WRITE_PRIMITIVE

void YAMLSerializer::beginObject(const char* name)
{
    if (mode.top() == Mode::Object)
        emitter << YAML::Key << (name ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
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
        this->key = true;
}

void YAMLSerializer::beginArray(size_t length, const char* name)
{
    if (mode.top() == Mode::Object)
        emitter << YAML::Key << (name ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
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
        this->key = true;
}

void YAMLSerializer::beginDictionary(size_t length, const char* name)
{
    if (mode.top() == Mode::Object)
        emitter << YAML::Key << (name ? name : ANONYMOUS_FIELD_NAME) << YAML::Value;
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
        this->key = true;
}
