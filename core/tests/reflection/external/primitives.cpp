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

    test<signed char>("signed char", INT8_MAX);
    test<short>("short", INT16_MIN);
    test<int>("int", INT16_MAX);
    test<long>("long", INT32_MIN);
    test<long long>("long long", INT64_MAX);

    test<unsigned char>("unsigned char", UINT8_MAX);
    test<unsigned short>("unsigned short", UINT16_MAX);
    test<unsigned int>("unsigned int", UINT16_MAX);
    test<unsigned long>("unsigned long", UINT32_MAX);
    test<unsigned long long>("unsigned long long", UINT64_MAX);

    test<float>("float", 3.14159F);
    test<double>("double", 0.123456789);
}
