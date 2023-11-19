#include <doctest/doctest.h>

#include <cubos/core/reflection/external/cstring.hpp>

#include "../traits/string_conversion.hpp"

TEST_CASE("reflection::reflect<const char*>()")
{
    CHECK(reflect<const char*>().name() == "const char*");
    testConstStringConversion<const char*>({{"", ""}, {"Hello World!", "Hello World!"}});
}

TEST_CASE("reflection::reflect<char*>()")
{
    CHECK(reflect<char*>().name() == "char*");

    char value[] = "Hello World!";
    testConstStringConversion<char*>({{value, "Hello World!"}});
}
