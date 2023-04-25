#include <cubos/core/data/json_deserializer.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

#define CHECK_ERROR(exp)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (this->failBit)                                                                                             \
        {                                                                                                              \
            CUBOS_WARN("Deserializer fail bit is set");                                                                \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            try                                                                                                        \
            {                                                                                                          \
                exp                                                                                                    \
            }                                                                                                          \
            catch (nlohmann::json::exception & e)                                                                      \
            {                                                                                                          \
                CUBOS_ERROR("Caught JSON exception '{}'", e.what());                                                   \
                this->failBit = true;                                                                                  \
            }                                                                                                          \
        }                                                                                                              \
    } while (false)

// NOLINTBEGIN(bugprone-macro-parentheses)
#define READ_GENERIC(out, type, fromStr)                                                                               \
    CHECK_ERROR(do {                                                                                                   \
        if (this->frame.top().mode == Mode::Dictionary)                                                                \
        {                                                                                                              \
            if (this->frame.top().key)                                                                                 \
            {                                                                                                          \
                out = static_cast<type>(fromStr(this->frame.top().iter.key()));                                        \
                this->frame.top().key = false;                                                                         \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                out = this->frame.top().iter.value();                                                                  \
                this->frame.top().iter++;                                                                              \
                this->frame.top().key = true;                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            out = this->frame.top().iter.value();                                                                      \
            this->frame.top().iter++;                                                                                  \
        }                                                                                                              \
    } while (false);)
// NOLINTEND(bugprone-macro-parentheses)

JSONDeserializer::JSONDeserializer(const std::string& src)
{
    CHECK_ERROR(do {
        this->json = nlohmann::ordered_json::array();
        this->json.push_back(nlohmann::ordered_json::parse(src));
        this->frame.push({Mode::Array, this->json.begin(), false});
    } while (false););
}

void JSONDeserializer::readI8(int8_t& value)
{
    READ_GENERIC(value, int8_t, std::stoi);
}

void JSONDeserializer::readI16(int16_t& value)
{
    READ_GENERIC(value, int16_t, std::stoi);
}

void JSONDeserializer::readI32(int32_t& value)
{
    READ_GENERIC(value, int32_t, std::stol);
}

void JSONDeserializer::readI64(int64_t& value)
{
    READ_GENERIC(value, int64_t, std::stoll);
}

void JSONDeserializer::readU8(uint8_t& value)
{
    READ_GENERIC(value, uint8_t, std::stoul);
}

void JSONDeserializer::readU16(uint16_t& value)
{
    READ_GENERIC(value, uint16_t, std::stoul);
}

void JSONDeserializer::readU32(uint32_t& value)
{
    READ_GENERIC(value, uint32_t, std::stoul);
}

void JSONDeserializer::readU64(uint64_t& value)
{
    READ_GENERIC(value, uint64_t, std::stoull);
}

void JSONDeserializer::readF32(float& value)
{
    READ_GENERIC(value, float, std::stof);
}

void JSONDeserializer::readF64(double& value)
{
    READ_GENERIC(value, double, std::stod);
}

void JSONDeserializer::readBool(bool& value)
{
    READ_GENERIC(value, bool, [](const std::string& s) { return s == "true"; });
}

void JSONDeserializer::readString(std::string& value)
{
    READ_GENERIC(value, std::string, [](const std::string& s) { return s; });
}

void JSONDeserializer::beginObject()
{
    assert(!this->frame.top().key); // Objects can't be used as keys.

    CHECK_ERROR(do {
        auto iter = this->frame.top().iter.value().begin();
        this->frame.push({Mode::Object, std::move(iter), false});
    } while (false););
}

void JSONDeserializer::endObject()
{
    assert(this->frame.size() > 1);

    CHECK_ERROR(do {
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

    CHECK_ERROR(do {
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

    CHECK_ERROR(do {
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

    CHECK_ERROR(do {
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

    CHECK_ERROR(do {
        this->frame.pop();
        this->frame.top().iter++;
        if (this->frame.top().mode == Mode::Dictionary)
        {
            this->frame.top().key = true;
        }
    } while (false););
}
