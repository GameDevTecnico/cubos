#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include "../traits/constructible.hpp"
#include "../traits/dictionary.hpp"

template <typename K, typename V>
static void test(const char* name, std::unordered_map<K, V> map, K insertedKey, V insertedValue)
{
    CHECK(reflect<std::unordered_map<K, V>>().name() == name);
    testDictionary<std::unordered_map<K, V>, K, V>(map, map.size(), &insertedKey, &insertedValue);
    testConstructible<std::unordered_map<K, V>>(&map);
}

TEST_CASE("reflection::reflect<std::unordered_map<K, V>>()")
{
    test<int, int>("std::unordered_map<int, int>", {{1, 2}, {2, 4}}, 3, 6);
    test<char, std::unordered_map<char, bool>>("std::unordered_map<char, std::unordered_map<char, bool>>",
                                               {
                                                   {'H', {{'i', true}, {'o', false}}},
                                               },
                                               'A', {{'h', true}});
}
