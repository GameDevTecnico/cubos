#include <doctest/doctest.h>

#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "../traits/constructible.hpp"
#include "../traits/dictionary.hpp"

template <typename K, typename V>
static void test(const char* name, std::map<K, V> map, K insertedKey, V insertedValue)
{
    CHECK(reflect<std::map<K, V>>().name() == name);
    testDictionary<std::map<K, V>, K, V>(map, map.size(), &insertedKey, &insertedValue);
    testConstructible<std::map<K, V>>(&map);
}

TEST_CASE("reflection::reflect<std::map<K, V>>()")
{
    test<int32_t, int32_t>("std::map<int32_t, int32_t>", {{1, 2}, {2, 4}}, 3, 6);
    test<char, std::map<char, bool>>("std::map<char, std::map<char, bool>>",
                                     {
                                         {'H', {{'i', true}, {'o', false}}},
                                     },
                                     'A', {{'h', true}});
}
