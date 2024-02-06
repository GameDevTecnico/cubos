#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/log.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::Type;

// Macro used to reduce code duplication for primitive type hooks.
#define AUTO_HOOK(type, ...)                                                                                           \
    this->hook<type>([this](const type& value) {                                                                       \
        mJSON = value;                                                                                                 \
        return true;                                                                                                   \
    })

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::reflection::reflect;

// Create hooks for primitive types.
JSONSerializer::JSONSerializer()
{
    // Boolean type.
    AUTO_HOOK(bool);

    // Char type (distinct from `signed char` and `unsigned char`).
    AUTO_HOOK(char);

    // Signed integer types.
    AUTO_HOOK(signed char);
    AUTO_HOOK(short);
    AUTO_HOOK(int);
    AUTO_HOOK(long);
    AUTO_HOOK(long long);

    // Unsigned integer types.
    AUTO_HOOK(unsigned char);
    AUTO_HOOK(unsigned short);
    AUTO_HOOK(unsigned int);
    AUTO_HOOK(unsigned long);
    AUTO_HOOK(unsigned long long);

    // Floating point types.
    AUTO_HOOK(float);
    AUTO_HOOK(double);

    // String
    AUTO_HOOK(std::string);
}

nlohmann::json JSONSerializer::output()
{
    return mJSON;
}

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
                CUBOS_WARN("Could not serialize array element");
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
        return true;
    }

    if (type.has<FieldsTrait>())
    {
        if (type.get<FieldsTrait>().size() == 1)
        {
            // If there's a single field, write it directly.
            if (!this->write(type.get<FieldsTrait>().begin()->type(),
                             type.get<FieldsTrait>().view(value).begin()->value))
            {
                CUBOS_WARN("Couldn't serialize wrapped field '{}'", type.get<FieldsTrait>().begin()->name());
                return false;
            }

            return true;
        }
        auto jsonObj = nlohmann::json::object();

        for (const auto& [field, fieldValue] : type.get<FieldsTrait>().view(value))
        {
            if (!this->write(field->type(), fieldValue))
            {
                CUBOS_WARN("Could not serialize field '{}'", field->name());
                return false;
            }
            jsonObj[field->name()] = mJSON;
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
                return false;
            }
            auto keyVal = mJSON;

            if (!this->write(trait.valueType(), entryValue))
            {
                CUBOS_WARN("Could not write dictionary value");
                return false;
            }
            // need to transform the key into a string (except if it already is)
            // nlohmann::json::object() only accepts strings as keys
            if (!keyVal.is_string())
            {
                keyVal = keyVal.dump();
            }
            jsonObj[keyVal] = mJSON;
        }
        mJSON = jsonObj;
        return true;
    }

    CUBOS_WARN("Cannot decompose '{}'", type.name());
    return false;
}