#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>

#include "../traits/constructible.hpp"

template <typename T>
static void test(const char* name, T value)
{
    CHECK(reflect<T>().name() == name);
    testConstructible<T>(&value);
}

TEST_CASE("reflection::reflect<(primitives)>()")
{
    test<bool>("bool", true);
    test<char>("char", 'A');
    test<int8_t>("int8_t", INT8_MAX);
    test<int16_t>("int16_t", INT16_MIN);
    test<int32_t>("int32_t", INT32_MAX);
    test<int64_t>("int64_t", INT64_MIN);
    test<uint8_t>("uint8_t", UINT8_MAX);
    test<uint16_t>("uint16_t", UINT16_MAX);
    test<uint32_t>("uint32_t", UINT32_MAX);
    test<uint64_t>("uint64_t", UINT64_MAX);
    test<float>("float", 3.14159F);
    test<double>("double", 0.123456789);
}
