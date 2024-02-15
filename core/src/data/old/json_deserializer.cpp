#include <cubos/core/data/old/json_deserializer.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>

using namespace cubos::core;
using namespace cubos::core::data::old;

#define CHECK_ERROR(exp)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mFailBit)                                                                                                  \
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
                CUBOS_ERROR("Caught JSON exception {}", e.what());                                                     \
                mFailBit = true;                                                                                       \
            }                                                                                                          \
        }                                                                                                              \
    } while (false)

// NOLINTBEGIN(bugprone-macro-parentheses)
#define READ_GENERIC(out, type, fromStr)                                                                               \
    CHECK_ERROR(do {                                                                                                   \
        if (mFrame.top().mode == Mode::Dictionary)                                                                     \
        {                                                                                                              \
            if (mFrame.top().key)                                                                                      \
            {                                                                                                          \
                out = static_cast<type>(fromStr(mFrame.top().iter.key()));                                             \
                mFrame.top().key = false;                                                                              \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                out = mFrame.top().iter.value();                                                                       \
                mFrame.top().iter++;                                                                                   \
                mFrame.top().key = true;                                                                               \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            out = mFrame.top().iter.value();                                                                           \
            mFrame.top().iter++;                                                                                       \
        }                                                                                                              \
    } while (false);)
// NOLINTEND(bugprone-macro-parentheses)

JSONDeserializer::JSONDeserializer(const std::string& src)
{
    CHECK_ERROR(do {
        mJson = nlohmann::ordered_json::array();
        mJson.push_back(nlohmann::ordered_json::parse(src));
        mFrame.push({Mode::Array, mJson.begin(), false});
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
    assert(!mFrame.top().key); // Objects can't be used as keys.

    CHECK_ERROR(do {
        auto iter = mFrame.top().iter.value().begin();
        mFrame.push({Mode::Object, std::move(iter), false});
    } while (false););
}

void JSONDeserializer::endObject()
{
    assert(mFrame.size() > 1);

    CHECK_ERROR(do {
        mFrame.pop();
        mFrame.top().iter++;
        if (mFrame.top().mode == Mode::Dictionary)
        {
            mFrame.top().key = true;
        }
    } while (false););
}

std::size_t JSONDeserializer::beginArray()
{
    assert(!mFrame.top().key); // Arrays can't be used as keys.

    CHECK_ERROR(do {
        auto iter = mFrame.top().iter.value().begin();
        auto len = mFrame.top().iter.value().size();
        mFrame.push({Mode::Array, std::move(iter), false});
        return len;
    } while (false););

    return 0;
}

void JSONDeserializer::endArray()
{
    assert(mFrame.size() > 1);

    CHECK_ERROR(do {
        mFrame.pop();
        mFrame.top().iter++;
        if (mFrame.top().mode == Mode::Dictionary)
        {
            mFrame.top().key = true;
        }
    } while (false););
}

std::size_t JSONDeserializer::beginDictionary()
{
    assert(!mFrame.top().key); // Dictionaries can't be used as keys.

    CHECK_ERROR(do {
        auto iter = mFrame.top().iter.value().begin();
        auto len = mFrame.top().iter.value().size();
        mFrame.push({Mode::Dictionary, std::move(iter), true});
        return len;
    } while (false););

    return 0;
}

void JSONDeserializer::endDictionary()
{
    assert(mFrame.size() > 1);

    CHECK_ERROR(do {
        mFrame.pop();
        mFrame.top().iter++;
        if (mFrame.top().mode == Mode::Dictionary)
        {
            mFrame.top().key = true;
        }
    } while (false););
}
