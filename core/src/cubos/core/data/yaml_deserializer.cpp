#include <cassert>

#include <cubos/core/data/yaml_deserializer.hpp>

using namespace cubos::core::data;

YAMLDeserializer::YAMLDeserializer(memory::Stream& stream)
    : stream(stream)
{
    this->loadDocument();
    this->frame.push({Mode::Object, this->document.begin(), false});
}

#define READ_PRIMITIVE(T, value)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        auto iter = this->get();                                                                                       \
        if (this->frame.top().mode == Mode::Dictionary)                                                                \
        {                                                                                                              \
            if (this->frame.top().key)                                                                                 \
                value = iter->first.as<T>(T());                                                                        \
            else                                                                                                       \
                value = iter->second.as<T>(T());                                                                       \
            this->frame.top().key = !this->frame.top().key;                                                            \
        }                                                                                                              \
        else if (this->frame.top().mode == Mode::Object)                                                               \
            value = iter->second.as<T>(T());                                                                           \
        else                                                                                                           \
            value = iter->as<T>(T());                                                                                  \
    } while (false)

void YAMLDeserializer::readI8(int8_t& value)
{
    int16_t v;
    READ_PRIMITIVE(int16_t, v);
    if (v < INT8_MIN || v > INT8_MAX)
        value = 0;
    else
        value = static_cast<int8_t>(v);
}

void YAMLDeserializer::readI16(int16_t& value)
{
    READ_PRIMITIVE(int16_t, value);
}

void YAMLDeserializer::readI32(int32_t& value)
{
    READ_PRIMITIVE(int32_t, value);
}

void YAMLDeserializer::readI64(int64_t& value)
{
    READ_PRIMITIVE(int64_t, value);
}

void YAMLDeserializer::readU8(uint8_t& value)
{
    uint16_t v;
    READ_PRIMITIVE(uint16_t, v);
    if (v > UINT8_MAX)
        value = 0;
    else
        value = static_cast<uint8_t>(v);
}

void YAMLDeserializer::readU16(uint16_t& value)
{
    READ_PRIMITIVE(uint16_t, value);
}

void YAMLDeserializer::readU32(uint32_t& value)
{
    READ_PRIMITIVE(uint32_t, value);
}

void YAMLDeserializer::readU64(uint64_t& value)
{
    READ_PRIMITIVE(uint64_t, value);
}

void YAMLDeserializer::readF32(float& value)
{
    READ_PRIMITIVE(float, value);
}

void YAMLDeserializer::readF64(double& value)
{
    READ_PRIMITIVE(double, value);
}

void YAMLDeserializer::readBool(bool& value)
{
    READ_PRIMITIVE(bool, value);
}

void YAMLDeserializer::readString(std::string& value)
{
    READ_PRIMITIVE(std::string, value);
}

void YAMLDeserializer::beginObject()
{
    auto iter = this->get();
    if (this->frame.top().mode == Mode::Array)
    {
        assert(iter->IsMap());
        this->frame.push({Mode::Object, iter->begin(), false});
    }
    else
    {
        // Objects can't be used as keys in dictionaries.
        assert(this->frame.top().mode != Mode::Dictionary || !this->frame.top().key);
        assert(iter->second.IsMap());
        this->frame.push({Mode::Object, iter->second.begin(), false});
    }
}

void YAMLDeserializer::endObject()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
    this->frame.top().key = true;
}

size_t YAMLDeserializer::beginArray()
{
    auto iter = this->get();
    if (this->frame.top().mode == Mode::Array)
    {
        assert(iter->IsSequence());
        this->frame.push({Mode::Array, iter->begin(), false});
        return iter->size();
    }
    else
    {
        // Arrays can't be used as keys in dictionaries.
        assert(this->frame.top().mode != Mode::Dictionary || !this->frame.top().key);
        assert(iter->second.IsSequence());
        this->frame.push({Mode::Array, iter->second.begin(), false});
        return iter->second.size();
    }
}

void YAMLDeserializer::endArray()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
    this->frame.top().key = true;
}

size_t YAMLDeserializer::beginDictionary()
{
    auto iter = this->get();
    if (this->frame.top().mode == Mode::Array)
    {
        assert(iter->IsMap());
        this->frame.push({Mode::Dictionary, iter->begin(), true});
        return iter->size();
    }
    else
    {
        // Dictionaries can't be used as keys in dictionaries.
        assert(this->frame.top().mode != Mode::Dictionary || !this->frame.top().key);
        if (iter->second.IsNull())
        {
            this->frame.push({Mode::Dictionary, iter->second.begin(), true});
            return 0;
        }
        else
        {
            assert(iter->second.IsMap());
            this->frame.push({Mode::Dictionary, iter->second.begin(), true});
            return iter->second.size();
        }
    }
}

void YAMLDeserializer::endDictionary()
{
    assert(this->frame.size() > 1);
    this->frame.pop();
    this->frame.top().key = true;
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

    if (this->frame.top().mode != Mode::Dictionary || !this->frame.top().key)
        return this->frame.top().iter++;
    else
        return this->frame.top().iter;
}
