#include <cubos/core/data/json_serializer.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core::data;

JSONSerializer::JSONSerializer(memory::Stream& stream, int indent) : Serializer(stream), indent(indent)
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
    this->frames.push({Mode::Object, nlohmann::ordered_json::object(), nullptr, name ? name : ""});
}

void JSONSerializer::endObject()
{
    assert(!this->frames.empty() && this->frames.top().mode == Mode::Object); // endObject without matching beginObject.
    auto json = std::move(this->frames.top().json);
    auto name = std::move(this->frames.top().name);
    this->frames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::beginArray(size_t length, const char* name)
{
    this->frames.push({Mode::Array, nlohmann::ordered_json::array(), nullptr, name ? name : ""});
}

void JSONSerializer::endArray()
{
    assert(!this->frames.empty() && this->frames.top().mode == Mode::Array); // endArray without matching beginArray.
    auto json = std::move(this->frames.top().json);
    auto name = std::move(this->frames.top().name);
    this->frames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::beginDictionary(size_t length, const char* name)
{
    this->frames.push({Mode::Dictionary, nlohmann::ordered_json::object(), nullptr, name ? name : ""});
}

void JSONSerializer::endDictionary()
{
    assert(!this->frames.empty() &&
           this->frames.top().mode == Mode::Dictionary); // endDictionary without matching beginDictionary.
    auto json = std::move(this->frames.top().json);
    auto name = std::move(this->frames.top().name);
    this->frames.pop();
    this->writeJSON(std::move(json), name.empty() ? nullptr : name.c_str());
}

void JSONSerializer::writeJSON(nlohmann::ordered_json&& json, const char* name)
{
    if (this->frames.empty())
    {
        this->stream.print(json.dump(this->indent));
    }
    else if (this->frames.top().mode == Mode::Dictionary)
    {
        if (this->frames.top().key.is_null())
        {
            if (json.is_string())
            {
                this->frames.top().key = std::move(json);
            }
            else
            {
                this->frames.top().key = json.dump();
            }
        }
        else
        {
            this->frames.top().json.emplace(std::move(this->frames.top().key), std::move(json));
            this->frames.top().key = nullptr;
        }
    }
    else if (this->frames.top().mode == Mode::Array)
    {
        this->frames.top().json.emplace_back(std::move(json));
    }
    else if (this->frames.top().mode == Mode::Object)
    {
        if (name == nullptr)
        {
            logCritical(
                "JSONSerializer::writeJSON() failed: tried to serialize object field without name, which is required");
            abort();
        }

        this->frames.top().json.emplace(name, std::move(json));
    }
}
