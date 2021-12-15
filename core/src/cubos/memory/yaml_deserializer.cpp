#include <cubos/memory/yaml_deserializer.hpp>

using namespace cubos::memory;

YAMLDeserializer::YAMLDeserializer(Stream& stream) : Deserializer(stream)
{
    this->loadDocument();
    this->frame.push({Mode::Object, this->document.begin()});
    this->key = true;
}

#define READ_PRIMITIVE(T, value)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        auto iter = this->get();                                                                                      \
        if (this->frame.top().mode == Mode::Dictionary)                                                                \
        {                                                                                                              \
            if (this->key)                                                                                             \
                value = iter->first.as<int8_t>();                                                                       \
            else                                                                                                       \
                value = iter->second.as<int8_t>();                                                                      \
            this->key = !this->key;                                                                                    \
        }                                                                                                              \
        else if (this->frame.top().mode == Mode::Object)                                                               \
            value = iter->second.as<int8_t>();                                                                          \
        else                                                                                                           \
            value = iter->as<int8_t>();                                                                                 \
    } while (false)

void YAMLDeserializer::read(int8_t& value)
{
    READ_PRIMITIVE(int8_t, value);
}

void YAMLDeserializer::read(int16_t& value)
{
    READ_PRIMITIVE(int16_t, value);
}

void YAMLDeserializer::read(int32_t& value)
{
    READ_PRIMITIVE(int32_t, value);
}

void YAMLDeserializer::read(int64_t& value)
{
    READ_PRIMITIVE(int64_t, value);
}

void YAMLDeserializer::read(uint8_t& value)
{
    READ_PRIMITIVE(uint8_t, value);
}

void YAMLDeserializer::read(uint16_t& value)
{
    READ_PRIMITIVE(uint16_t, value);
}

void YAMLDeserializer::read(uint32_t& value)
{
    READ_PRIMITIVE(uint32_t, value);
}

void YAMLDeserializer::read(uint64_t& value)
{
    READ_PRIMITIVE(uint64_t, value);
}

void YAMLDeserializer::read(float& value)
{
    READ_PRIMITIVE(float, value);
}

void YAMLDeserializer::read(double& value)
{
    READ_PRIMITIVE(double, value);
}

void YAMLDeserializer::read(bool& value)
{
    READ_PRIMITIVE(bool, value);
}

void YAMLDeserializer::read(std::string& value)
{
    READ_PRIMITIVE(std::string, value);
}

void YAMLDeserializer::beginObject()
{
    auto& frame = this->frame.top();
    assert(frame.iter->IsMap());
    this->frame.push({Mode::Object, (frame.iter++)->begin()});
}

void YAMLDeserializer::endObject()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
}

size_t YAMLDeserializer::beginArray()
{
    auto& frame = this->frame.top();
    assert(frame.iter->IsSequence());
    this->frame.push({Mode::Array, frame.iter->begin()});
    return (frame.iter++)->size();
}

void YAMLDeserializer::endArray()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
}

size_t YAMLDeserializer::beginDictionary()
{
    auto& frame = this->frame.top();
    assert(frame.iter->IsMap());
    this->frame.push({Mode::Dictionary, frame.iter->begin()});
    return (frame.iter++)->size();
}

void YAMLDeserializer::endDictionary()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
}

void YAMLDeserializer::loadDocument()
{
    std::string content;
    this->stream.readUntil(content, "...");
    this->document = YAML::Load(content);
}

YAML::const_iterator YAMLDeserializer::get()
{
    // If this is the top frame and there aren't more elements to read, read another document.
    if (this->frame.size() == 1 && this->frame.top().iter == this->document.end())
    {
        this->loadDocument();
        this->frame.top().iter = this->document.begin();
    }

    return this->frame.top().iter++;
}
