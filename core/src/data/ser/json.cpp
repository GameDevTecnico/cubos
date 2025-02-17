#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/traits/wrapper.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;
using cubos::core::reflection::WrapperTrait;

// Macro used to reduce code duplication for primitive type hooks.
#define AUTO_HOOK(type, ...)                                                                                           \
    this->hook<type>([this](const type& value) {                                                                       \
        mJSON = value;                                                                                                 \
        return true;                                                                                                   \
    })

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

bool JSONSerializer::decompose(const Type& type, const void* value)
{
    if (type.has<WrapperTrait>())
    {
        const auto& wrapper = type.get<WrapperTrait>();
        return this->write(wrapper.type(), wrapper.value(value));
    }

    if (type.has<NullableTrait>())
    {
        const auto& trait = type.get<NullableTrait>();

        if (trait.isNull(value))
        {
            mJSON = nlohmann::json::value_t::null;
            return true;
        }
    }

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

    if (type.has<EnumTrait>())
    {
        const auto& trait = type.get<EnumTrait>();
        mJSON = trait.variant(value).name();
        return true;
    }

    if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        auto view = trait.view(value);
        auto jsonObj = nlohmann::json::object();

        for (const auto& [field, fieldValue] : view)
        {
            if (!this->write(field->type(), fieldValue))
            {
                CUBOS_WARN("Could not serialize field {}", field->name());
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

    CUBOS_WARN("Cannot decompose {}", type.name());
    return false;
}