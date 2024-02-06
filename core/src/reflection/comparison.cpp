#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/comparison.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::memory::AnyValue;

#define PRIMITIVE_CHECK(reflected, type, a, b)                                                                         \
    if ((reflected).is<type>())                                                                                        \
    return *(type*)(a) == *(type*)(b)

bool cubos::core::reflection::compare(const Type& type, void* a, void* b)
{
    PRIMITIVE_CHECK(type, bool, a, b);
    PRIMITIVE_CHECK(type, char, a, b);
    PRIMITIVE_CHECK(type, signed char, a, b);
    PRIMITIVE_CHECK(type, short, a, b);
    PRIMITIVE_CHECK(type, int, a, b);
    PRIMITIVE_CHECK(type, long, a, b);
    PRIMITIVE_CHECK(type, long long, a, b);
    PRIMITIVE_CHECK(type, unsigned char, a, b);
    PRIMITIVE_CHECK(type, unsigned short, a, b);
    PRIMITIVE_CHECK(type, unsigned int, a, b);
    PRIMITIVE_CHECK(type, unsigned long, a, b);
    PRIMITIVE_CHECK(type, unsigned long long, a, b);
    PRIMITIVE_CHECK(type, float, a, b);
    PRIMITIVE_CHECK(type, double, a, b);

    if (type.has<FieldsTrait>())
    {
        for (const auto& trait = type.get<FieldsTrait>(); const auto& field : trait)
        {
            if (!compare(field.type(), trait.view(a).get(field), trait.view(b).get(field)))
            {
                return false;
            }
        }
        return true;
    }

    if (type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();
        if (trait.view(a).length() != trait.view(b).length())
        {
            return false;
        }
        for (size_t i = 0; i < trait.view(a).length(); i++)
        {
            if (!compare(trait.elementType(), trait.view(a).get(i), trait.view(b).get(i)))
            {
                return false;
            }
        }
        return true;
    }

    if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();
        if (trait.view(a).length() != trait.view(b).length())
        {
            return false;
        }
        for (const auto& [key, value] : trait.view(a))
        {
            if (trait.view(b).find(key) == trait.view(b).end())
            {
                return false;
            }
            if (!compare(trait.valueType(), value, trait.view(b).find(key)->value))
            {
                return false;
            }
        }
        return true;
    }

    if (type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();
        return trait.into(a) == trait.into(b);
    }

    CUBOS_FAIL("Type cannot be compared!");
}
