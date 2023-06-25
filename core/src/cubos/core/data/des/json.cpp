#include <sstream>

#include <cubos/core/data/des/json.hpp>
#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/dictionary.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/variant.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::reflection::reflect;

#define EXPECT_OR_RETURN(condition, expected, found)                                                                   \
    if (!(condition))                                                                                                  \
    {                                                                                                                  \
        CUBOS_ERROR("Expected {}, found {}", (expected), (found));                                                     \
        return false;                                                                                                  \
    }

#define AUTO_HOOK(type, condition, expected)                                                                           \
    this->hook<type>([this](void* data) {                                                                              \
        if (mKey == nullptr)                                                                                           \
        {                                                                                                              \
            EXPECT_OR_RETURN((condition), expected, mValue->type_name());                                            \
            mValue->get_to<type>(*static_cast<type*>(data));                                                         \
        }                                                                                                              \
        else if (&reflect<type>() == &reflect<std::string>())                                                          \
        {                                                                                                              \
            *static_cast<std::string*>(data) = *mKey;                                                                  \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            std::stringstream ss(*mKey);                                                                               \
            ss >> std::boolalpha >> *static_cast<type*>(data);                                                         \
            if (ss.fail())                                                                                             \
            {                                                                                                          \
                CUBOS_ERROR("Failed to parse key '{}' as {}", *mKey, reflect<type>().name());                          \
                return false;                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        return true;                                                                                                   \
    })

JSONDeserializer::JSONDeserializer()
    : mAllowsMissingFields(false)
    , mKey(nullptr)
    , mValue(&mJSON)
{
    AUTO_HOOK(bool, mValue->is_boolean(), "JSON boolean");
    AUTO_HOOK(int8_t, mValue->is_number_integer(), "JSON number (integer)");
    AUTO_HOOK(int16_t, mValue->is_number_integer(), "JSON number (integer)");
    AUTO_HOOK(int32_t, mValue->is_number_integer(), "JSON number (integer)");
    AUTO_HOOK(int64_t, mValue->is_number_integer(), "JSON number (integer)");
    AUTO_HOOK(uint8_t, mValue->is_number_unsigned(), "JSON number (unsigned)");
    AUTO_HOOK(uint16_t, mValue->is_number_unsigned(), "JSON number (unsigned)");
    AUTO_HOOK(uint32_t, mValue->is_number_unsigned(), "JSON number (unsigned)");
    AUTO_HOOK(uint64_t, mValue->is_number_unsigned(), "JSON number (unsigned)");
    AUTO_HOOK(float, mValue->is_number_float(), "JSON number (float)");
    AUTO_HOOK(double, mValue->is_number_float(), "JSON number (float)");
    AUTO_HOOK(std::string, mValue->is_string(), "JSON string");
}

bool JSONDeserializer::parse(std::string&& json)
{
    try
    {
        this->feed(nlohmann::json::parse(std::move(json)));
        return true;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        CUBOS_ERROR("Failed to parse JSON: {}", e.what());
        return false;
    }
}

void JSONDeserializer::feed(nlohmann::json&& json)
{
    mJSON = std::move(json);
}

void JSONDeserializer::allowsMissingFields(bool allow)
{
    mAllowsMissingFields = allow;
}

bool JSONDeserializer::readKey(const std::string& key, const reflection::Type& type, void* data)
{
    mKey = &key;
    auto result = this->read(type, data);
    mKey = nullptr;
    return result;
}

bool JSONDeserializer::readValue(const nlohmann::json* json, const reflection::Type& type, void* data)
{
    auto old = mValue;
    mValue = json;
    auto result = this->read(type, data);
    mValue = old;
    return result;
}

bool JSONDeserializer::readObject(const reflection::ObjectType& type, void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON deserializer does not support using objects as keys");
    EXPECT_OR_RETURN(mValue->is_object(), "JSON object for object type", mValue->type_name());

    for (const auto& field : type.fields())
    {
        if (auto it = mValue->find(field->name()); it != mValue->end())
        {
            if (!this->readValue(&*it, field->type(), field->get(data)))
            {
                CUBOS_ERROR("Failed to read field '{}' of type {} on object of type {}", field->name(),
                            field->type().name(), type.name());
                return false;
            }
        }
        else if (!mAllowsMissingFields)
        {
            CUBOS_ERROR("Missing field '{}' of type {} for object of type {}", field->name(), field->type().name(),
                        type.name());
            return false;
        }
    }

    // Check if there are any extra fields.
    for (auto it = mValue->begin(); it != mValue->end(); ++it)
    {
        bool found = false;
        for (const auto& field : type.fields())
        {
            if (it.key() == field->name())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            CUBOS_ERROR("No such field '{}' on object of type {}", it.key(), type.name());
            return false;
        }
    }

    return true;
}

bool JSONDeserializer::readVariant(const reflection::VariantType& type, void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON deserializer does not support using variants as keys");
    EXPECT_OR_RETURN(mValue->is_object(), "JSON object for variant type", mValue->type_name());

    // Variants in JSON are encoded as objects with two fields: "type" and "value".
    auto typeIt = mValue->find("type");
    auto valueIt = mValue->find("value");
    if (mValue->size() != 2 || typeIt == mValue->end() || valueIt == mValue->end())
    {
        CUBOS_ERROR("Expected JSON object with only two fields \"type\" and \"value\" for variant type, got {}",
                    mValue->dump());
        return false;
    }

    // Check if the type field is a string.
    if (!typeIt->is_string())
    {
        CUBOS_ERROR("Expected JSON string for variant type field, got {}", typeIt->type_name());
        return false;
    }

    // Check if the type field is a valid type name.
    for (const auto& variant : type.variants())
    {
        if (variant.type().name() == *typeIt)
        {
            // Read the value field.
            variant.emplaceDefault(data);
            if (!this->readValue(&*valueIt, variant.type(), variant.get(data)))
            {
                CUBOS_ERROR("Failed to read variant value of type {} for variant type {}", variant.type().name(),
                            type.name());
                return false;
            }

            return true;
        }
    }

    CUBOS_ERROR("No such variant type {} for variant type {}", typeIt->get<std::string>(), type.name());
    return false;
}

bool JSONDeserializer::readArray(const reflection::ArrayType& type, void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON deserializer does not support using arrays as keys");
    EXPECT_OR_RETURN(mValue->is_array(), "JSON array for array type", mValue->type_name());

    // If the type is resizable, resize the array to fit the JSON array.
    // Otherwise, check if the JSON array is the correct size.
    if (type.resize() != nullptr)
    {
        type.resize(data, mValue->size());
    }
    else if (type.length(data) != mValue->size())
    {
        CUBOS_ERROR("Expected JSON array of length {}, got {}", type.length(data), mValue->size());
        return false;
    }

    for (std::size_t i = 0; i < mValue->size(); ++i)
    {
        if (!this->readValue(&(*mValue)[i], type.elementType(), type.element(data, i)))
        {
            CUBOS_ERROR("Failed to read array element {} of type {} for array of type {}", i, type.elementType().name(),
                        type.name());
            return false;
        }
    }

    return true;
}

bool JSONDeserializer::readDictionary(const reflection::DictionaryType& type, void* data)
{
    CUBOS_ASSERT(mKey == nullptr, "JSON deserializer does not support using dictionaries as keys");
    EXPECT_OR_RETURN(mValue->is_object(), "JSON object for dictionary type", mValue->type_name());

    type.clear(data);

    void* key = type.keyType().defaultConstruct();
    for (auto it = mValue->begin(); it != mValue->end(); ++it)
    {
        if (!this->readKey(it.key(), type.keyType(), key))
        {
            CUBOS_ERROR("Failed to read dictionary key of type {}", type.keyType().name());
            type.keyType().destroy(key);
            return false;
        }

        void* value = type.insert(data, key);
        if (!this->readValue(&*it, type.valueType(), value))
        {
            CUBOS_ERROR("Failed to read dictionary value of type {} for key '{}'", type.valueType().name(), it.key());
            type.keyType().destroy(key);
            return false;
        }
    }

    type.keyType().destroy(key);
    return true;
}
