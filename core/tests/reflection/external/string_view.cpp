#include <doctest/doctest.h>

#include <cubos/core/reflection/external/string_view.hpp>

#include "../traits/string_conversion.hpp"

TEST_CASE("reflection::reflect<std::string_view>()")
{
    CHECK(reflect<std::string_view>().name() == "std::string_view");
    testConstStringConversion<std::string_view>({{"", ""}, {"Hello World!", "Hello World!"}});
}
