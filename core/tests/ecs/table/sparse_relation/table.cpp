#include <algorithm>
#include <map>
#include <set>

#include <doctest/doctest.h>

#include <cubos/core/ecs/table/sparse_relation/table.hpp>

#include "../../utils.hpp"

using cubos::core::ecs::SparseRelationTable;
using cubos::core::reflection::reflect;

template <typename V>
static std::map<std::pair<uint32_t, uint32_t>, uintptr_t> collectRelations(const V& view)
{
    std::map<std::pair<uint32_t, uint32_t>, uintptr_t> map{};
    for (const auto& [from, to, value] : view)
    {
        auto pair = std::make_pair(from, to);
        REQUIRE_FALSE(map.contains(pair));
        map.emplace(pair, value);
    }
    return map;
}

TEST_CASE("ecs::SparseRelationTable") // NOLINT(readability-function-size)
{
    SUBCASE("zero-sized relation type")
    {
        EmptyRelation empty{}; // Doesn't store anything, thus can be reused.
        SparseRelationTable table{reflect<EmptyRelation>()};
        REQUIRE(table.size() == 0);

        REQUIRE_FALSE(table.insert(0, 1, &empty));
        REQUIRE(table.size() == 1);
        REQUIRE_FALSE(table.insert(1, 0, &empty));
        REQUIRE(table.size() == 2);
        REQUIRE(table.insert(0, 1, &empty)); // Overwrites existing relation
        REQUIRE(table.size() == 2);
        REQUIRE_FALSE(table.insert(0, 2, &empty));
        REQUIRE(table.size() == 3);
        REQUIRE_FALSE(table.insert(0, 0, &empty));
        REQUIRE(table.size() == 4);

        REQUIRE(table.contains(0, 1));
        REQUIRE(table.contains(1, 0));
        REQUIRE(table.contains(0, 2));
        REQUIRE(table.contains(0, 0));
        REQUIRE_FALSE(table.contains(2, 0));
        REQUIRE_FALSE(table.contains(1, 1));

        REQUIRE(table.erase(0, 0));
        REQUIRE_FALSE(table.contains(0, 0));
        REQUIRE(table.size() == 3);
        REQUIRE_FALSE(table.erase(0, 0));
        REQUIRE_FALSE(table.erase(2, 0));

        auto map = collectRelations(table);
        CHECK(map.size() == 3);
        CHECK(map.contains({0, 1}));
        CHECK(map.contains({0, 2}));
        CHECK(map.contains({1, 0}));

        map = collectRelations(table.viewFrom(0));
        CHECK(map.size() == 2);
        CHECK(map.contains({0, 1}));
        CHECK(map.contains({0, 2}));

        map = collectRelations(table.viewFrom(1));
        CHECK(map.size() == 1);
        CHECK(map.contains({1, 0}));

        map = collectRelations(table.viewFrom(2));
        CHECK(map.empty());

        map = collectRelations(table.viewTo(1));
        CHECK(map.size() == 1);
        CHECK(map.contains({0, 1}));

        map = collectRelations(table.viewTo(3));
        CHECK(map.empty());
    }

    SUBCASE("relation with destructor detection")
    {
        bool flag1 = false;
        bool flag2 = false;
        bool flag3 = false;

        DetectDestructorRelation detector1{{&flag1}};
        DetectDestructorRelation detector2{{&flag2}};
        DetectDestructorRelation detector3{{&flag3}};

        {
            SparseRelationTable table{reflect<DetectDestructorRelation>()};
            REQUIRE(table.size() == 0);

            table.insert(1000, 2000, &detector1);
            table.insert(2000, 4000, &detector2);
            table.insert(4000, 8000, &detector3);

            REQUIRE_FALSE(flag1);
            REQUIRE_FALSE(flag2);
            REQUIRE_FALSE(flag3);

            SUBCASE("no erases")
            {
            }

            SUBCASE("erase first")
            {
                REQUIRE(table.erase(1000, 2000));
                REQUIRE(flag1);
                REQUIRE_FALSE(flag2);
                REQUIRE_FALSE(flag3);
            }

            SUBCASE("erase middle")
            {
                REQUIRE(table.erase(2000, 4000));
                REQUIRE_FALSE(flag1);
                REQUIRE(flag2);
                REQUIRE_FALSE(flag3);
            }

            SUBCASE("erase last")
            {
                REQUIRE(table.erase(4000, 8000));
                REQUIRE_FALSE(flag1);
                REQUIRE_FALSE(flag2);
                REQUIRE(flag3);
            }

            SUBCASE("erase all")
            {
                REQUIRE(table.erase(1000, 2000));
                REQUIRE(table.erase(2000, 4000));
                REQUIRE(table.erase(4000, 8000));
                REQUIRE(flag1);
                REQUIRE(flag2);
                REQUIRE(flag3);
            }
        }

        // Table has been deconstructed, all of the relations should also have been deconstructed.
        CHECK(flag1);
        CHECK(flag2);
        CHECK(flag3);
    }

    SUBCASE("stress")
    {
        bool useEraseFromTo = false;
        PARAMETRIZE_TRUE_OR_FALSE("with eraseFrom and eraseTo", useEraseFromTo);

        constexpr std::size_t ListSize = 100;
        constexpr std::size_t ListCount = 10;

        SparseRelationTable table{reflect<IntegerRelation>()};
        REQUIRE(table.size() == 0);

        for (uint32_t i = 1; i <= static_cast<uint32_t>(ListCount); ++i)
        {
            for (uint32_t j = 1; j <= static_cast<uint32_t>(ListSize); ++j)
            {
                IntegerRelation rel{.value = static_cast<int>(i * ListCount + j)};
                REQUIRE_FALSE(table.insert(i, j, &rel));
            }
        }

        REQUIRE(table.size() == ListSize * ListCount);

        // Remove relations with odd 'to' index.
        for (uint32_t j = 1; j <= static_cast<uint32_t>(ListSize); j += 2)
        {
            if (useEraseFromTo)
            {
                REQUIRE(table.eraseTo(j) == ListCount);
                REQUIRE(table.eraseTo(j) == 0);
            }
            else
            {
                for (uint32_t i = 1; i <= static_cast<uint32_t>(ListCount); ++i)
                {
                    REQUIRE(table.erase(i, j));
                }
            }
        }

        // Check if the number of relations with each 'from' is correct.
        for (uint32_t i = 1; i <= static_cast<uint32_t>(ListCount); ++i)
        {
            auto map = collectRelations(table.viewFrom(i));
            REQUIRE(map.size() == ListSize / 2); // Removed the odd elements.
        }

        // Check if the number of relations with each 'to' is correct.
        for (uint32_t j = 1; j <= static_cast<uint32_t>(ListSize); ++j)
        {
            auto map = collectRelations(table.viewTo(j));
            if (j % 2 == 0)
            {
                REQUIRE(map.size() == ListCount);
            }
            else
            {
                // We deleted all relations with odd 'to's.
                REQUIRE(map.empty());
            }
        }

        // Remove relations with even 'from' index.
        for (uint32_t i = 2; i <= static_cast<uint32_t>(ListCount); i += 2)
        {
            if (useEraseFromTo)
            {
                REQUIRE(table.eraseFrom(i) == ListSize / 2);
                REQUIRE(table.eraseFrom(i) == 0);
            }
            else
            {
                for (uint32_t j = 1; j <= static_cast<uint32_t>(ListSize); ++j)
                {
                    if (j % 2 == 0)
                    {
                        REQUIRE(table.erase(i, j));
                    }
                    else
                    {
                        // Was already deleted due to 'to' being odd.
                        REQUIRE_FALSE(table.erase(i, j));
                    }
                }
            }
        }

        // Check if the number of relations with each 'from' is correct.
        for (uint32_t i = 1; i <= static_cast<uint32_t>(ListCount); ++i)
        {
            auto map = collectRelations(table.viewFrom(i));
            if (i % 2 == 0)
            {
                REQUIRE(map.empty()); // Removed the even 'from's.
            }
            else
            {
                REQUIRE(map.size() == ListSize / 2); // Removed the odd elements.
            }
        }

        // Check if the number of relations with each 'to' is correct.
        for (uint32_t j = 1; j <= static_cast<uint32_t>(ListSize); ++j)
        {
            auto map = collectRelations(table.viewTo(j));
            if (j % 2 == 0)
            {
                REQUIRE(map.size() == ListCount / 2); // Removed the even elements.
            }
            else
            {
                // We deleted all relations with odd 'to's.
                REQUIRE(map.empty());
            }
        }
    }

    SUBCASE("moving from one table to another")
    {
        bool flag1 = false;
        bool flag2 = false;
        DetectDestructorRelation detector1{{&flag1}};
        DetectDestructorRelation detector2{{&flag2}};

        SparseRelationTable srcTable{reflect<DetectDestructorRelation>()};
        SparseRelationTable dstTable{reflect<DetectDestructorRelation>()};

        // Insert two relations into the source table.
        REQUIRE_FALSE(srcTable.insert(1, 2, &detector1));
        REQUIRE_FALSE(srcTable.insert(2, 1, &detector2));
        REQUIRE(srcTable.contains(1, 2));
        REQUIRE(srcTable.contains(2, 1));

        // Move the relations with from index 1 to the destination table.
        REQUIRE(srcTable.moveFrom(1, dstTable));
        REQUIRE_FALSE(flag1);
        REQUIRE_FALSE(flag2);
        REQUIRE_FALSE(srcTable.contains(1, 2));
        REQUIRE(srcTable.contains(2, 1));
        REQUIRE(dstTable.contains(1, 2));
        REQUIRE_FALSE(dstTable.contains(2, 1));

        // Move the relations with to index 1 to the destination table.
        REQUIRE(srcTable.moveTo(1, dstTable));
        REQUIRE_FALSE(flag1);
        REQUIRE_FALSE(flag2);
        REQUIRE_FALSE(srcTable.contains(1, 2));
        REQUIRE_FALSE(srcTable.contains(2, 1));
        REQUIRE(dstTable.contains(1, 2));
        REQUIRE(dstTable.contains(2, 1));

        // Erase both relations.
        REQUIRE(dstTable.erase(1, 2));
        REQUIRE(flag1);
        REQUIRE_FALSE(flag2);
        REQUIRE(dstTable.erase(2, 1));
        REQUIRE(flag2);
    }
}
