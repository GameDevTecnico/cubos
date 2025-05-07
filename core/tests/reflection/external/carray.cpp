#include <doctest/doctest.h>

#include <cubos/core/reflection/external/carray.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "../traits/array.hpp"
#include "../traits/constructible.hpp"

template <typename T, std::size_t size>
static void test(const char* name, T (&arr)[size])
{
    CHECK(reflect<T[size]>().name() == name);
    testArray<T[size], T>(arr, size);
    testConstructible<T[size]>(&arr);
}

TEST_CASE("reflection::reflect<T[size]>()")
{
    int simple[3] = {5, 4, 1};
    test<int, 3>("int[3]", simple);

    int nested[2][3] = {{5, 4, 1}, {2, 3, 0}};
    test<int[3], 2>("int[3][2]", nested);
}
