#include <cassert>

#include <cubos/core/data/yaml_deserializer.hpp>

using namespace cubos::core::data;

YAMLDeserializer::YAMLDeserializer(memory::Stream& stream)
    : mStream(stream)
{
    this->loadDocument();
    mFrame.push({Mode::Object, mDocument.begin(), false});
}

// NOLINTBEGIN(bugprone-macro-parentheses)
#define READ_PRIMITIVE(T, value)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        auto iter = this->get();                                                                                       \
        if (mFrame.top().mode == Mode::Dictionary)                                                                     \
        {                                                                                                              \
            if (mFrame.top().key)                                                                                      \
                value = iter->first.as<T>(T());                                                                        \
            else                                                                                                       \
                value = iter->second.as<T>(T());                                                                       \
            mFrame.top().key = !mFrame.top().key;                                                                      \
        }                                                                                                              \
        else if (mFrame.top().mode == Mode::Object)                                                                    \
            value = iter->second.as<T>(T());                                                                           \
        else                                                                                                           \
            value = iter->as<T>(T());                                                                                  \
    } while (false)
// NOLINTEND(bugprone-macro-parentheses)

void YAMLDeserializer::readI8(int8_t& value)
{
    int16_t v;
    READ_PRIMITIVE(int16_t, v);
    if (v < INT8_MIN || v > INT8_MAX)
    {
        value = 0;
    }
    else
    {
        value = static_cast<int8_t>(v);
    }
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
    {
        value = 0;
    }
    else
    {
        value = static_cast<uint8_t>(v);
    }
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
    if (mFrame.top().mode == Mode::Array)
    {
        assert(iter->IsMap());
        mFrame.push({Mode::Object, iter->begin(), false});
    }
    else
    {
        // Objects can't be used as keys in dictionaries.
        assert(mFrame.top().mode != Mode::Dictionary || !mFrame.top().key);
        assert(iter->second.IsMap());
        mFrame.push({Mode::Object, iter->second.begin(), false});
    }
}

void YAMLDeserializer::endObject()
{
    assert(mFrame.size() > 1);
    mFrame.pop();
    mFrame.top().key = true;
}

std::size_t YAMLDeserializer::beginArray()
{
    auto iter = this->get();
    if (mFrame.top().mode == Mode::Array)
    {
        assert(iter->IsSequence());
        mFrame.push({Mode::Array, iter->begin(), false});
        return iter->size();
    }

    // Arrays can't be used as keys in dictionaries.
    assert(mFrame.top().mode != Mode::Dictionary || !mFrame.top().key);
    assert(iter->second.IsSequence());
    mFrame.push({Mode::Array, iter->second.begin(), false});
    return iter->second.size();
}

void YAMLDeserializer::endArray()
{
    assert(mFrame.size() > 1);
    mFrame.pop();
    mFrame.top().key = true;
}

std::size_t YAMLDeserializer::beginDictionary()
{
    auto iter = this->get();
    if (mFrame.top().mode == Mode::Array)
    {
        assert(iter->IsMap());
        mFrame.push({Mode::Dictionary, iter->begin(), true});
        return iter->size();
    }

    // Dictionaries can't be used as keys in dictionaries.
    assert(mFrame.top().mode != Mode::Dictionary || !mFrame.top().key);
    if (iter->second.IsNull())
    {
        mFrame.push({Mode::Dictionary, iter->second.begin(), true});
        return 0;
    }

    assert(iter->second.IsMap());
    mFrame.push({Mode::Dictionary, iter->second.begin(), true});
    return iter->second.size();
}

void YAMLDeserializer::endDictionary()
{
    assert(mFrame.size() > 1);
    mFrame.pop();
    mFrame.top().key = true;
}

void YAMLDeserializer::loadDocument()
{
    std::string content;
    mStream.readUntil(content, "...");
    mDocument = YAML::Load(content);
}

YAML::const_iterator YAMLDeserializer::get()
{
    // If this is the top frame and there aren't more elements to read, read another document.
    if (mFrame.size() == 1 && mFrame.top().iter == mDocument.end())
    {
        this->loadDocument();
        mFrame.top().iter = mDocument.begin();
    }

    if (mFrame.top().mode != Mode::Dictionary || !mFrame.top().key)
    {
        return mFrame.top().iter++;
    }
    return mFrame.top().iter;
}
