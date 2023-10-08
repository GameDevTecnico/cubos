#include <doctest/doctest.h>

#include <cubos/core/memory/unordered_bimap.hpp>

TEST_CASE("memory::UnorderedBimap")
{
    using cubos::core::memory::UnorderedBimap;

    UnorderedBimap<int, char> bimap{};

    SUBCASE("just initialized")
    {
    }

    SUBCASE("had entries")
    {
        bimap.add(1, 'a');

        CHECK_FALSE(bimap.empty());
        CHECK(bimap.size() == 1);

        CHECK(bimap.has(1, 'a'));
        CHECK(bimap.hasLeft(1));
        CHECK(bimap.hasRight('a'));
        CHECK(bimap.getLeft('a') == 1);
        CHECK(bimap.getRight(1) == 'a');

        CHECK(bimap.begin() != bimap.end());
        CHECK(bimap.begin()->first == 1);
        CHECK(bimap.begin()->second == 'a');
        CHECK(++bimap.begin() == bimap.end());

        bimap.add(2, 'b');

        CHECK(bimap.size() == 2);
        CHECK(bimap.has(1, 'a'));
        CHECK(bimap.has(2, 'b'));
        CHECK(bimap.getRight(1) == 'a');
        CHECK(bimap.getRight(2) == 'b');
        CHECK(bimap.getLeft('a') == 1);
        CHECK(bimap.getLeft('b') == 2);

        bimap.add(1, 'b');

        CHECK(bimap.size() == 1);
        CHECK(bimap.has(1, 'b'));
        CHECK_FALSE(bimap.has(1, 'a'));
        CHECK_FALSE(bimap.has(2, 'b'));
        CHECK(bimap.getRight(1) == 'b');
        CHECK(bimap.getLeft('b') == 1);

        SUBCASE("remove left")
        {
            CHECK(bimap.removeLeft(1));
        }

        SUBCASE("remove right")
        {
            CHECK(bimap.removeRight('b'));
        }

        SUBCASE("clear")
        {
            bimap.clear();
        }
    }

    // Shouldn't contain anything
    CHECK(bimap.empty());
    CHECK(bimap.size() == 0);

    CHECK_FALSE(bimap.has(1, 'a'));
    CHECK_FALSE(bimap.hasLeft(1));
    CHECK_FALSE(bimap.hasRight('a'));

    CHECK(bimap.begin() == bimap.end());

    CHECK_FALSE(bimap.removeLeft(1));
    CHECK_FALSE(bimap.removeRight('a'));
}
