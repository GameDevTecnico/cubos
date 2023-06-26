#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/dictionary.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/variant.hpp>

using cubos::core::data::JSONSerializer;

#define AUTO_HOOK(type, toString)                                                                                      \
    this->hook<type>([this](const void* data) {                                                                        \
        if (mKey == nullptr)                                                                                           \
        {                                                                                                              \
            *mValue = *static_cast<const type*>(data);                                                                 \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            *mKey = toString(*static_cast<const type*>(data));                                                         \
        }                                                                                                              \
    })

JSONSerializer::JSONSerializer()
    : mKey(nullptr)
    , mValue(&mJSON)
{
    AUTO_HOOK(bool, [](bool value) { return value ? "true" : "false"; });
    AUTO_HOOK(int8_t, std::to_string);
    AUTO_HOOK(int16_t, std::to_string);
    AUTO_HOOK(int32_t, std::to_string);
    AUTO_HOOK(int64_t, std::to_string);
    AUTO_HOOK(uint8_t, std::to_string);
    AUTO_HOOK(uint16_t, std::to_string);
    AUTO_HOOK(uint32_t, std::to_string);
    AUTO_HOOK(uint64_t, std::to_string);
    AUTO_HOOK(float, std::to_string);
    AUTO_HOOK(double, std::to_string);
    AUTO_HOOK(std::string, [](const std::string& value) { return value; });
}

nlohmann::json JSONSerializer::json()
{
    nlohmann::json json = std::move(mJSON);
    mJSON = nlohmann::json();
    mKey = nullptr;
    mValue = &mJSON;
    return json;
}

std::string JSONSerializer::string(int indent)
{
    return json().dump(indent);
}

void JSONSerializer::writeKey(std::string& key, const reflection::Type& type, const void* data)
{
    mKey = &key;
    this->write(type, data);
    mKey = nullptr;
}

void JSONSerializer::writeValue(nlohmann::json& json, const reflection::Type& type, const void* data)
{
    auto old = mValue;
    mValue = &json;
    this->write(type, data);
    mValue = old;
}

void JSONSerializer::writeObject(const reflection::ObjectType& type, const void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON serializer does not support using objects as keys");

    *mValue = nlohmann::json::object();
    for (const auto& field : type.fields())
    {
        this->writeValue((*mValue)[field->name()], field->type(), field->get(data));
    }
}

void JSONSerializer::writeVariant(const reflection::VariantType& type, const void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON serializer does not support using variants as keys");

    *mValue = nlohmann::json::object();
    auto variant = type.variant(data);
    (*mValue)["type"] = variant.type().name();
    this->writeValue((*mValue)["value"], variant.type(), variant.get(data));
}

void JSONSerializer::writeArray(const reflection::ArrayType& type, const void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON serializer does not support using arrays as keys");

    *mValue = nlohmann::json::array();
    for (size_t i = 0, length = type.length(data); i < length; ++i)
    {
        this->writeValue((*mValue)[i], type.elementType(), type.element(data, i));
    }
}

void JSONSerializer::writeDictionary(const reflection::DictionaryType& type, const void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON serializer does not support using dictionaries as keys");

    *mValue = nlohmann::json::object();
    for (auto iterator = type.iterator(data); iterator.valid(); iterator.increment(data))
    {
        std::string key;
        this->writeKey(key, type.keyType(), iterator.key(data));
        this->writeValue((*mValue)[key], type.valueType(), iterator.value(data));
    }
}
