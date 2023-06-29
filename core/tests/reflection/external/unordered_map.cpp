#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include "../utils.hpp"

using cubos::core::reflection::DictionaryType;
using cubos::core::reflection::reflect;

template <typename K, typename V>
static void testStaticInfo(const char* name)
{
    testTypeGetters<std::unordered_map<K, V>, DictionaryType>(name, "std::unordered_map");
    testTypeDefaultConstructor<std::unordered_map<K, V>>();
    testTypeMoveConstructor<std::unordered_map<K, V>>();

    // Check if it holds the correct element type.
    auto& td = reflect<std::unordered_map<K, V>>().template asKind<DictionaryType>();
    REQUIRE(td.keyType().template is<K>());
    REQUIRE(td.valueType().template is<V>());
}

TEST_CASE("reflection::reflect<std::unordered_map>()")
{
    SUBCASE("static information is correct")
    {
        testStaticInfo<int32_t, bool>("std::unordered_map<int32_t, bool>");
        testStaticInfo<bool, std::unordered_map<char, float>>(
            "std::unordered_map<bool, std::unordered_map<char, float>>");
    }

    SUBCASE("inspect and modify an unordered map")
    {
        std::unordered_map<int32_t, bool> map = {{1, false}, {2, true}};
        auto& t = reflect<decltype(map)>().asKind<DictionaryType>();

        // Check if the map contains the correct elements, and if they can be modified.
        REQUIRE(t.length(&map) == 2);
        CHECK(t.value<int32_t, bool>(&map, 0) == nullptr);
        CHECK(*t.value<int32_t, bool>(&map, 1) == false);
        CHECK(*t.value<int32_t, bool>(&map, 2) == true);
        *t.value<int32_t, bool>(&map, 1) = true;
        CHECK(map.at(1) == true);

        // Inserting an already existing key returns the existing value.
        CHECK(t.insert<int32_t, bool>(&map, 1) == true);

        // Inserting a new key returns a new default constructed value.
        CHECK(t.insert<int32_t, bool>(&map, 0) == bool{});
        CHECK(map.at(0) == bool{});
        CHECK(t.length(&map) == 3);

        // Removing an existing key returns true.
        CHECK(t.remove<int32_t>(&map, 1) == true);
        CHECK(t.length(&map) == 2);
        CHECK(t.value<int32_t, bool>(&map, 1) == nullptr);

        // Removing a non-existing key returns false.
        CHECK(t.remove<int32_t>(&map, 1) == false);
        CHECK(t.length(&map) == 2);

        // Clearing the map removes all elements.
        t.clear(&map);
        CHECK(t.length(&map) == 0);
    }

    SUBCASE("iterate over an unordered map")
    {
        std::unordered_map<bool, int32_t> map = {};
        auto& t = reflect<decltype(map)>().asKind<DictionaryType>();

        SUBCASE("empty")
        {
        }

        SUBCASE("not empty")
        {
            map[false] = 1;
            map[true] = 2;
        }

        // Iterate over the map.
        std::unordered_map<bool, int32_t> found = {};
        for (auto it = t.iterator(&map); it.valid(); it.increment(&map))
        {
            auto key = it.key<bool>(&map);
            auto value = it.value<int32_t>(&map);
            CHECK_FALSE(found.contains(key));
            found[key] = value;
        }

        // Check if all elements were found.
        CHECK(found.size() == map.size());
        for (auto [key, value] : map)
        {
            CHECK(found.at(key) == value);
        }
    }
}
