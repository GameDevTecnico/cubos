#include <cubos/core/data/json_deserializer.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

#define CHECK_ERROR(name, exp)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (this->failBit)                                                                                             \
        {                                                                                                              \
            logWarning("JSONDeserializer::{}() failed: deserializer fail bit is set", name);                           \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            try                                                                                                        \
            {                                                                                                          \
                exp                                                                                                    \
            }                                                                                                          \
            catch (nlohmann::json::exception e)                                                                        \
            {                                                                                                          \
                logError("JSONDeserializer::{}() failed: caught JSON exception '{}'", name, e.what());                 \
                this->failBit = true;                                                                                  \
            }                                                                                                          \
        }                                                                                                              \
    } while (false)

#define READ_GENERIC(name, out, fromStr)                                                                               \
    CHECK_ERROR(                                                                                                       \
        name, do {                                                                                                     \
            if (this->frame.top().mode == Mode::Dictionary)                                                            \
            {                                                                                                          \
                if (this->frame.top().key)                                                                             \
                {                                                                                                      \
                    out = fromStr(this->frame.top().iter.key());                                                       \
                    this->frame.top().key = false;                                                                     \
                }                                                                                                      \
                else                                                                                                   \
                {                                                                                                      \
                    out = this->frame.top().iter.value();                                                              \
                    this->frame.top().iter++;                                                                          \
                    this->frame.top().key = true;                                                                      \
                }                                                                                                      \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                out = this->frame.top().iter.value();                                                                  \
                this->frame.top().iter++;                                                                              \
            }                                                                                                          \
        } while (false);)

JSONDeserializer::JSONDeserializer(const std::string& src)
{
    CHECK_ERROR(
        "JSONDeserializer", do {
            this->json = nlohmann::ordered_json::array();
            this->json.push_back(nlohmann::ordered_json::parse(src));
            this->frame.push({Mode::Array, this->json.begin(), false});
        } while (false););
}

void JSONDeserializer::readI8(int8_t& value)
{
    READ_GENERIC("readI8", value, std::stoi);
}

void JSONDeserializer::readI16(int16_t& value)
{
    READ_GENERIC("readI16", value, std::stoi);
}

void JSONDeserializer::readI32(int32_t& value)
{
    READ_GENERIC("readI32", value, std::stol);
}

void JSONDeserializer::readI64(int64_t& value)
{
    READ_GENERIC("readI64", value, std::stoll);
}

void JSONDeserializer::readU8(uint8_t& value)
{
    READ_GENERIC("readU8", value, std::stoul);
}

void JSONDeserializer::readU16(uint16_t& value)
{
    READ_GENERIC("readU16", value, std::stoul);
}

void JSONDeserializer::readU32(uint32_t& value)
{
    READ_GENERIC("readU32", value, std::stoul);
}

void JSONDeserializer::readU64(uint64_t& value)
{
    READ_GENERIC("readU64", value, std::stoull);
}

void JSONDeserializer::readF32(float& value)
{
    READ_GENERIC("readF32", value, std::stof);
}

void JSONDeserializer::readF64(double& value)
{
    READ_GENERIC("readF64", value, std::stod);
}

void JSONDeserializer::readBool(bool& value)
{
    READ_GENERIC("readBool", value, [](const std::string& s) { return s == "true"; });
}

void JSONDeserializer::readString(std::string& value)
{
    READ_GENERIC("readString", value, [](const std::string& s) { return s; });
}

void JSONDeserializer::beginObject()
{
    assert(!this->frame.top().key); // Objects can't be used as keys.

    CHECK_ERROR(
        "beginObject", do {
            auto iter = this->frame.top().iter.value().begin();
            this->frame.push({Mode::Object, std::move(iter), false});
        } while (false););
}

void JSONDeserializer::endObject()
{
    assert(this->frame.size() > 1);

    CHECK_ERROR(
        "endObject", do {
            this->frame.pop();
            this->frame.top().iter++;
            if (this->frame.top().mode == Mode::Dictionary)
            {
                this->frame.top().key = true;
            }
        } while (false););
}

size_t JSONDeserializer::beginArray()
{
    assert(!this->frame.top().key); // Arrays can't be used as keys.

    CHECK_ERROR(
        "beginArray", do {
            auto iter = this->frame.top().iter.value().begin();
            auto len = this->frame.top().iter.value().size();
            this->frame.push({Mode::Array, std::move(iter), false});
            return len;
        } while (false););

    return 0;
}

void JSONDeserializer::endArray()
{
    assert(this->frame.size() > 1);

    CHECK_ERROR(
        "endArray", do {
            this->frame.pop();
            this->frame.top().iter++;
            if (this->frame.top().mode == Mode::Dictionary)
            {
                this->frame.top().key = true;
            }
        } while (false););
}

size_t JSONDeserializer::beginDictionary()
{
    assert(!this->frame.top().key); // Dictionaries can't be used as keys.

    CHECK_ERROR(
        "beginDictionary", do {
            auto iter = this->frame.top().iter.value().begin();
            auto len = this->frame.top().iter.value().size();
            this->frame.push({Mode::Dictionary, std::move(iter), true});
            return len;
        } while (false););

    return 0;
}

void JSONDeserializer::endDictionary()
{
    assert(this->frame.size() > 1);

    CHECK_ERROR(
        "endDictionary", do {
            this->frame.pop();
            this->frame.top().iter++;
            if (this->frame.top().mode == Mode::Dictionary)
            {
                this->frame.top().key = true;
            }
        } while (false););
}
