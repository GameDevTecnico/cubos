#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include "../traits/array.hpp"
#include "../traits/constructible.hpp"

template <typename T>
static void test(const char* name, std::vector<T> vec, T inserted)
{
    CHECK(reflect<std::vector<T>>().name() == name);
    testArray<std::vector<T>, T>(vec, vec.size(), &inserted);
    testConstructible<std::vector<T>>(&vec);
}

TEST_CASE("reflection::reflect<std::vector<T>>()")
{
    test<int>("std::vector<int>", {5, 4, 1}, 1);
    test<std::vector<std::vector<char>>>("std::vector<std::vector<std::vector<char>>>", {{{'H'}, {'e'}}, {{'y', '!'}}},
                                         {{'o'}});
}
