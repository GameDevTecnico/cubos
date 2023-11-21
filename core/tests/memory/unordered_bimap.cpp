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
        bimap.insert(1, 'a');

        CHECK_FALSE(bimap.empty());
        CHECK(bimap.size() == 1);

        CHECK(bimap.contains(1, 'a'));
        CHECK(bimap.containsLeft(1));
        CHECK(bimap.containsRight('a'));
        CHECK(bimap.atRight('a') == 1);
        CHECK(bimap.atLeft(1) == 'a');

        CHECK(bimap.begin() != bimap.end());
        CHECK(bimap.begin()->first == 1);
        CHECK(bimap.begin()->second == 'a');
        CHECK(++bimap.begin() == bimap.end());

        bimap.insert(2, 'b');

        CHECK(bimap.size() == 2);
        CHECK(bimap.contains(1, 'a'));
        CHECK(bimap.contains(2, 'b'));
        CHECK(bimap.atLeft(1) == 'a');
        CHECK(bimap.atLeft(2) == 'b');
        CHECK(bimap.atRight('a') == 1);
        CHECK(bimap.atRight('b') == 2);

        bimap.insert(1, 'b');

        CHECK(bimap.size() == 1);
        CHECK(bimap.contains(1, 'b'));
        CHECK_FALSE(bimap.contains(1, 'a'));
        CHECK_FALSE(bimap.contains(2, 'b'));
        CHECK(bimap.atLeft(1) == 'b');
        CHECK(bimap.atRight('b') == 1);

        SUBCASE("remove left")
        {
            CHECK(bimap.eraseLeft(1));
        }

        SUBCASE("remove right")
        {
            CHECK(bimap.eraseRight('b'));
        }

        SUBCASE("clear")
        {
            bimap.clear();
        }
    }

    // Shouldn't contain anything
    CHECK(bimap.empty());
    CHECK(bimap.size() == 0);

    CHECK_FALSE(bimap.contains(1, 'a'));
    CHECK_FALSE(bimap.containsLeft(1));
    CHECK_FALSE(bimap.containsRight('a'));

    CHECK(bimap.begin() == bimap.end());

    CHECK_FALSE(bimap.eraseLeft(1));
    CHECK_FALSE(bimap.eraseRight('a'));
}
