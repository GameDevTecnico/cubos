#include <cubos/core/data/old/json_serializer.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core::data::old;

JSONSerializer::JSONSerializer(memory::Stream& stream, int indent)
    : mStream(stream)
    , mIndent(indent)
{
    // Do nothing.
}

void JSONSerializer::writeI8(int8_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeI16(int16_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeI32(int32_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeI64(int64_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeU8(uint8_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeU16(uint16_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeU32(uint32_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeU64(uint64_t value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeF32(float value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeF64(double value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeBool(bool value, const char* name)
{
    this->writeJSON(value, name);
}

void JSONSerializer::writeString(const char* value, const char* name)
{
    assert(value != nullptr);
    this->writeJSON(value, name);
}

#undef WRITE_PRIMITIVE

void JSONSerializer::beginObject(const char* name)
{
    mFrames.push({Mode::Object, nlohmann::ordered_json::object(), nullptr, name != nullptr ? name : ""});
}

void JSONSerializer::endObject()
{
    assert(!mFrames.empty() && mFrames.top().mode == Mode::Object); // endObject without matching beginObject.
    auto json = std::move(mFrames.top().json);
    auto name = std::move(mFrames.top().name);
    mFrames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::beginArray(std::size_t /*length*/, const char* name)
{
    mFrames.push({Mode::Array, nlohmann::ordered_json::array(), nullptr, name != nullptr ? name : ""});
}

void JSONSerializer::endArray()
{
    assert(!mFrames.empty() && mFrames.top().mode == Mode::Array); // endArray without matching beginArray.
    auto json = std::move(mFrames.top().json);
    auto name = std::move(mFrames.top().name);
    mFrames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::beginDictionary(std::size_t /*length*/, const char* name)
{
    mFrames.push({Mode::Dictionary, nlohmann::ordered_json::object(), nullptr, name != nullptr ? name : ""});
}

void JSONSerializer::endDictionary()
{
    assert(!mFrames.empty() &&
           mFrames.top().mode == Mode::Dictionary); // endDictionary without matching beginDictionary.
    auto json = std::move(mFrames.top().json);
    auto name = std::move(mFrames.top().name);
    mFrames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::writeJSON(nlohmann::ordered_json&& json, const char* name)
{
    if (mFrames.empty())
    {
        mStream.print(json.dump(mIndent));
    }
    else if (mFrames.top().mode == Mode::Dictionary)
    {
        if (mFrames.top().key.is_null())
        {
            if (json.is_string())
            {
                mFrames.top().key = std::move(json);
            }
            else
            {
                mFrames.top().key = json.dump();
            }
        }
        else
        {
            mFrames.top().json.emplace(std::move(mFrames.top().key), std::move(json));
            mFrames.top().key = nullptr;
        }
    }
    else if (mFrames.top().mode == Mode::Array)
    {
        mFrames.top().json.emplace_back(std::move(json));
    }
    else if (mFrames.top().mode == Mode::Object)
    {
        if (name == nullptr)
        {
            CUBOS_CRITICAL("Objects serialized with JSONSerializer must name their fields");
            abort();
        }

        mFrames.top().json.emplace(name, std::move(json));
    }
}
