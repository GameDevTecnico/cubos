#include <doctest/doctest.h>

#include <cubos/core/ecs/table/dense.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "../utils.hpp"

using cubos::core::ecs::DenseColumnId;
using cubos::core::ecs::DenseTable;
using cubos::core::reflection::reflect;

TEST_CASE("ecs::DenseTable")
{
    SUBCASE("without columns")
    {
        DenseTable table{};
        REQUIRE(table.size() == 0);

        table.pushBack(0);
        table.pushBack(1);
        table.pushBack(2);

        REQUIRE(table.size() == 3);
        REQUIRE(table.row(0) == 0);
        REQUIRE(table.row(1) == 1);
        REQUIRE(table.row(2) == 2);
        REQUIRE(table.entity(0) == 0);
        REQUIRE(table.entity(1) == 1);
        REQUIRE(table.entity(2) == 2);

        table.swapErase(0);

        REQUIRE(table.size() == 2);
        REQUIRE(table.row(1) == 1);
        REQUIRE(table.row(2) == 0);
        REQUIRE(table.entity(0) == 2);
        REQUIRE(table.entity(1) == 1);

        table.swapErase(1);

        REQUIRE(table.size() == 1);
        REQUIRE(table.row(2) == 0);
        REQUIRE(table.entity(0) == 2);

        table.swapErase(2);

        REQUIRE(table.size() == 0);
    }

    SUBCASE("with a column")
    {
        int zero = 0;
        int one = 1;
        int two = 2;

        DenseTable table{};
        table.addColumn({.inner = 0}, reflect<int>());
        auto& col = table.column({.inner = 0});

        REQUIRE(table.size() == 0);

        table.pushBack(0);
        col.pushCopy(&zero);
        table.pushBack(1);
        col.pushCopy(&one);
        table.pushBack(2);
        col.pushCopy(&two);

        REQUIRE(table.size() == 3);
        REQUIRE(*static_cast<int*>(col.at(0)) == zero);
        REQUIRE(*static_cast<int*>(col.at(1)) == one);
        REQUIRE(*static_cast<int*>(col.at(2)) == two);

        table.swapErase(0);

        REQUIRE(table.size() == 2);
        REQUIRE(*static_cast<int*>(col.at(0)) == two);
        REQUIRE(*static_cast<int*>(col.at(1)) == one);

        table.swapErase(1);

        REQUIRE(table.size() == 1);
        REQUIRE(*static_cast<int*>(col.at(0)) == two);
    }

    SUBCASE("destructors are called correctly")
    {
        bool flags[3] = {false, false, false};
        DetectDestructor detectors[3] = {&flags[0], &flags[1], &flags[2]};

        {
            DenseTable table{};
            table.addColumn({.inner = 0}, reflect<DetectDestructor>());
            auto& col = table.column({.inner = 0});

            table.pushBack(0);
            col.pushMove(&detectors[0]);
            table.pushBack(1);
            col.pushMove(&detectors[1]);
            table.pushBack(2);
            col.pushMove(&detectors[2]);

            REQUIRE_FALSE(flags[0]);
            REQUIRE_FALSE(flags[1]);
            REQUIRE_FALSE(flags[2]);

            table.swapErase(0);

            REQUIRE(flags[0]);
            REQUIRE_FALSE(flags[1]);
            REQUIRE_FALSE(flags[2]);

            table.swapErase(2);

            REQUIRE(flags[0]);
            REQUIRE_FALSE(flags[1]);
            REQUIRE(flags[2]);
        }

        REQUIRE(flags[0]);
        REQUIRE(flags[1]);
        REQUIRE(flags[2]);
    }

    SUBCASE("move from one table into another")
    {
        bool flags[2] = {false, false};
        DetectDestructor detectors[2] = {&flags[0], &flags[1]};

        DenseTable table1{};
        table1.addColumn({.inner = 0}, reflect<DetectDestructor>());
        table1.addColumn({.inner = 1}, reflect<DetectDestructor>());

        DenseTable table2{};
        table2.addColumn({.inner = 0}, reflect<DetectDestructor>());

        table1.pushBack(0);
        table1.column({.inner = 0}).pushMove(&detectors[0]);
        table1.column({.inner = 1}).pushMove(&detectors[1]);

        REQUIRE_FALSE(flags[0]);
        REQUIRE_FALSE(flags[1]);

        table1.swapMove(0, table2);

        REQUIRE(table1.size() == 0);
        REQUIRE(table2.size() == 1);

        REQUIRE(table2.entity(0) == 0);
        REQUIRE_FALSE(flags[0]);
        REQUIRE(flags[1]);

        table2.swapErase(0);

        REQUIRE(table2.size() == 0);
        REQUIRE(flags[0]);
        REQUIRE(flags[1]);
    }
}