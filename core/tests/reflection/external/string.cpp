#include <doctest/doctest.h>

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>

#include "../traits/constructible.hpp"
#include "../traits/string_conversion.hpp"

TEST_CASE("reflection::reflect<std::string>()")
{
    std::string value = "Hello, world!";
    CHECK(reflect<std::string>().name() == "std::string");
    testConstructible<std::string>(&value);
    testStringConversion<std::string>({{"", ""}, {"Hello World!", "Hello World!"}}, {});
}
