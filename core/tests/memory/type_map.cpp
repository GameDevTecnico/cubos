#include <doctest/doctest.h>

#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

TEST_CASE("memory::TypeMap")
{
    using cubos::core::memory::TypeMap;
    using cubos::core::reflection::reflect;

    TypeMap<int> map{};

    SUBCASE("just initialized")
    {
    }

    SUBCASE("had entries")
    {
        map.insert<bool>(1);
        CHECK_FALSE(map.empty());
        CHECK(map.size() == 1);

        CHECK(map.contains<bool>());
        CHECK(map.at<bool>() == 1);

        CHECK(map.begin() != map.end());
        CHECK(map.begin()->first == &reflect<bool>());
        CHECK(map.begin()->second == 1);
        CHECK(++map.begin() == map.end());

        map.insert<bool>(2);
        CHECK_FALSE(map.empty());
        CHECK(map.size() == 1);
        CHECK(map.at<bool>() == 2);

        map.insert<float>(4);
        map.insert<double>(8);
        CHECK_FALSE(map.empty());
        CHECK(map.size() == 3);
        CHECK(static_cast<const TypeMap<int>&>(map).at<bool>() == 2);
        CHECK(map.at<float>() == 4);
        CHECK(map.at<double>() == 8);

        SUBCASE("remove")
        {
            CHECK(map.erase<bool>());
            CHECK_FALSE(map.erase<bool>());
            CHECK_FALSE(map.empty());
            CHECK(map.size() == 2);
            CHECK_FALSE(map.contains<bool>());
            CHECK(map.contains<float>());
            CHECK(map.contains<double>());

            CHECK(map.erase<float>());
            CHECK(map.erase<double>());
        }

        SUBCASE("clear")
        {
            map.clear();
        }
    }

    // Shouldn't contain anything
    CHECK(map.empty());
    CHECK(map.size() == 0);

    CHECK_FALSE(map.contains<bool>());
    CHECK_FALSE(map.contains<float>());
    CHECK_FALSE(map.contains<double>());

    CHECK(map.begin() == map.end());

    CHECK_FALSE(map.erase<bool>());
    CHECK_FALSE(map.erase<float>());
    CHECK_FALSE(map.erase<double>());
}
