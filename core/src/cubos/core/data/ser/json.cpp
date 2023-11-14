#include <cubos/core/log.hpp>

/// [JSON serializer]
#include <cubos/core/data/ser/json.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::Type;

auto jsonVal = nlohmann::json::object();

/// [MACRO to add hooks for primitive types]
#define AUTO_HOOK(type, ...)                                                                                           \
    this->hook<type>([](const type& value) {                                                                           \
        jsonVal = value;                                                                                               \
        return true;                                                                                                   \
    })

/// [Setting up hooks]
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::reflection::reflect;

JSONSerializer::JSONSerializer()
{
    AUTO_HOOK(bool);
    AUTO_HOOK(char);
    AUTO_HOOK(int8_t);
    AUTO_HOOK(int16_t);
    AUTO_HOOK(int32_t);
    AUTO_HOOK(int64_t);
    AUTO_HOOK(uint8_t);
    AUTO_HOOK(uint16_t);
    AUTO_HOOK(uint32_t);
    AUTO_HOOK(uint64_t);
    AUTO_HOOK(float);
    AUTO_HOOK(double);
    AUTO_HOOK(std::string);
}
/// [Setting up hooks]

nlohmann::json JSONSerializer::output()
{
    return mJSON;
}

/// [Decomposing types]
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::StringConversionTrait;

bool JSONSerializer::decompose(const Type& type, const void* value)
{
    if (type.has<ArrayTrait>())
    {
        auto jsonArr = nlohmann::json::array();

        const auto& arrayTrait = type.get<ArrayTrait>();

        for (const auto* element : arrayTrait.view(value))
        {
            if (!this->write(arrayTrait.elementType(), element))
            {
                CUBOS_WARN("Could not deserialize array element");
                return false;
            }
            jsonArr.push_back(mJSON);
        }

        mJSON = jsonArr;
        return true;
    }

    if (type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();
        mJSON = trait.into(value);
    }

    if (type.has<FieldsTrait>())
    {
        auto jsonObj = nlohmann::json::object();

        for (const auto& [field, fieldValue] : type.get<FieldsTrait>().view(value))
        {
            if (!this->write(field->type(), fieldValue))
            {
                return false;
            }
            jsonObj[field->name()] = jsonVal;
        }
        mJSON = jsonObj;
        return true;
    }

    if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();

        auto jsonObj = nlohmann::json::object();

        for (auto [entryKey, entryValue] : trait.view(value))
        {
            if (!this->write(trait.keyType(), entryKey))
            {
                CUBOS_WARN("Could not write dictionary key");
            }
            auto keyVal = jsonVal;

            if (!this->write(trait.valueType(), entryValue))
            {
                CUBOS_WARN("Could not write dictionary value");
            }
            jsonObj[keyVal] = jsonVal;
        }
    }

    CUBOS_WARN("Cannot decompose '{}'", type.name());
    return false;
}