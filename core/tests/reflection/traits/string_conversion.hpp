#include <algorithm>
#include <initializer_list>

#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::reflect;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

/// @brief Checks if a type's StringConversionTrait works as expected.
/// @tparam T Type.
/// @param correct Correct string representations for each given value.
/// @param invalid Invalid string representations.
template <typename T>
void testStringConversion(std::initializer_list<std::pair<T, std::string>> correct,
                          std::initializer_list<std::string> invalid)
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<StringConversionTrait>());
    const StringConversionTrait& trait = type.get<StringConversionTrait>();

    for (const auto& [value, string] : correct)
    {
        CHECK(trait.into(&value) == string);
        T result{};
        CHECK(trait.from(&result, string));
        CHECK(result == value);
    }

    for (const auto& string : invalid)
    {
        T result{};
        CHECK_FALSE(trait.from(&result, string));
    }
}

/// @brief Checks if a type's StringConversionTrait works as expected for a const type.
/// @tparam T Type.
/// @param expected Expected string representations for each given value.
template <typename T>
void testConstStringConversion(std::initializer_list<std::pair<T, std::string>> expected)
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<StringConversionTrait>());
    const StringConversionTrait& trait = type.get<StringConversionTrait>();

    for (const auto& [value, string] : expected)
    {
        CHECK(trait.into(&value) == string);
    }
}
