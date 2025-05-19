#include <doctest/doctest.h>

#include <cubos/core/reflection/external/carray.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "../traits/array.hpp"
#include "../traits/constructible.hpp"

template <typename T, std::size_t Size>
static void test(const char* name, T (&arr)[Size])
{
    CHECK(reflect<T[Size]>().name() == name);
    testArray<T[Size], T>(arr, Size);
    testConstructible<T[Size]>(&arr);
}

TEST_CASE("reflection::reflect<T[Size]>()")
{
    int simple[3] = {5, 4, 1};
    test<int, 3>("int[3]", simple);

    int nested[2][3] = {{5, 4, 1}, {2, 3, 0}};
    test<int[3], 2>("int[3][2]", nested);
}
