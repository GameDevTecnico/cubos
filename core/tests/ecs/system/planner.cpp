#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/planner.hpp>

#include "utils.hpp"

using namespace cubos::core::ecs;

TEST_CASE("ecs::Planner")
{
    // Prepare world and context.
    World world{};
    CommandBuffer cmdBuffer{world};
    SystemContext context{.cmdBuffer = cmdBuffer};

    // Register a vector which will store the system order, and a map which will be used to store whether conditions
    // should keep returning true.
    world.registerResource<std::vector<int>>();
    world.registerResource<std::map<int, int>>();
    world.insertResource(std::vector<int>{});
    world.insertResource(std::map<int, int>{});

    // Create some systems and conditions.
    SystemRegistry registry{};
    auto system1 = registry.add("system1", makeSystem(world, pushToOrder<1>));
    auto system2 = registry.add("system2", makeSystem(world, pushToOrder<2>));
    auto system3 = registry.add("system3", makeSystem(world, pushToOrder<3>));
    auto true5 = registry.add("true5", makeCondition(world, pushToOrderAndReturn<5, true>));
    auto false6 = registry.add("false6", makeCondition(world, pushToOrderAndReturn<6, false>));
    auto trueOnce7 = registry.add("trueOnce7", makeCondition(world, pushToOrderAndTrueNTimes<7, 1>));
    auto trueTwice8 = registry.add("trueTwice8", makeCondition(world, pushToOrderAndTrueNTimes<8, 2>));

    Planner planner{};
    std::vector<int> expected{};

    SUBCASE("simple systems run in default order")
    {
        planner.add(system1);
        planner.add(system2);
        planner.add(system3);

        expected = {3, 2, 1};
    }

    SUBCASE("same system can be added multiple times")
    {
        planner.add(system1);
        planner.add(system1);

        expected = {1, 1};
    }

    SUBCASE("ordering can be specified directly on leaf tags")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);
        planner.add(system3);

        CHECK(planner.order(leaf1, leaf2));

        SUBCASE("valid order")
        {
            SUBCASE("no redundancy")
            {
            }

            SUBCASE("redundancy")
            {
                CHECK(planner.order(leaf1, leaf2));
            }

            expected = {3, 1, 2};
        }

        CHECK_FALSE(planner.order(leaf2, leaf1)); // Can't introduce cyclic dependency.
    }

    SUBCASE("ordering is transitive")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);
        auto tag1 = planner.add();

        CHECK(planner.order(leaf1, tag1));
        CHECK(planner.order(tag1, leaf2));

        expected = {1, 2};
    }

    SUBCASE("conditions run with same constraints as their tags")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);
        planner.add(system3);

        CHECK(planner.order(leaf1, leaf2));
        planner.onlyIf(leaf2, true5);

        SUBCASE("single condition")
        {
            expected = {3, 1, 5, 2};
        }

        SUBCASE("two conditions")
        {
            planner.onlyIf(leaf2, true5);

            expected = {3, 1, 5, 5, 2};
        }
    }

    SUBCASE("ordering can be specified indirectly through parent tags")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);
        auto leaf3 = planner.add(system3);

        auto tag1 = planner.add();
        CHECK(planner.tag(leaf1, tag1));
        CHECK(planner.tag(leaf2, tag1));

        auto tag2 = planner.add();
        CHECK(planner.tag(leaf3, tag2));

        CHECK(planner.order(tag1, tag2));

        SUBCASE("no internal ordering")
        {
            expected = {2, 1, 3};
        }

        SUBCASE("with internal ordering")
        {
            CHECK(planner.order(leaf1, leaf2));

            expected = {1, 2, 3};
        }
    }

    SUBCASE("conditions affect entire tags")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);
        auto leaf3 = planner.add(system3);

        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        CHECK(planner.tag(leaf1, tag1));
        CHECK(planner.tag(leaf2, tag2));
        CHECK(planner.tag(tag1, tag3));
        CHECK(planner.tag(tag2, tag3));
        CHECK(planner.tag(leaf3, tag3));

        SUBCASE("evaluates to true")
        {
            planner.onlyIf(tag3, true5);

            expected = {5, 3, 2, 1};
        }

        SUBCASE("evaluates to false")
        {
            planner.onlyIf(tag3, false6);

            expected = {6};
        }
    }

    SUBCASE("systems and conditions within nested repeat tags")
    {
        auto leaf1 = planner.add(system1);
        auto leaf2 = planner.add(system2);

        auto tag1 = planner.add();
        auto tag2 = planner.add();

        planner.onlyIf(leaf2, true5);

        CHECK(planner.tag(leaf1, tag1));
        CHECK(planner.tag(leaf2, tag2));
        CHECK(planner.tag(tag2, tag1));

        CHECK(planner.repeatWhile(tag1, trueOnce7));
        CHECK(planner.repeatWhile(tag2, trueTwice8));

        expected = {7, 8, 1, 5, 2, 8, 5, 2, 8, 7};
    }

    SUBCASE("two repeating parents are forbidden")
    {
        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        CHECK(planner.repeatWhile(tag1, trueOnce7));
        CHECK_FALSE(planner.repeatWhile(tag1, trueTwice8)); // Can't specify two repeats for same tag.

        SUBCASE("fail in tag")
        {
            CHECK(planner.repeatWhile(tag2, trueTwice8));
            CHECK(planner.tag(tag3, tag1));
            CHECK_FALSE(planner.tag(tag3, tag2));
        }

        SUBCASE("fail in repeatWhile")
        {
            CHECK(planner.tag(tag3, tag1));
            CHECK(planner.tag(tag3, tag2));
            CHECK_FALSE(planner.repeatWhile(tag2, trueTwice8));
        }
    }

    SUBCASE("system runs before all other systems through negate")
    {
        auto leaf1 = planner.add(system1);
        planner.add(system2);
        planner.add(system3);

        CHECK(planner.negate(planner.negate(leaf1)) == leaf1);

        CHECK(planner.order(leaf1, planner.negate(leaf1)));

        // Cycle is detected correctly.
        CHECK_FALSE(planner.order(planner.negate(leaf1), leaf1));

        expected = {1, 3, 2};
    }

    SUBCASE("negated tags cannot be set to be repeating, directly or indirectly")
    {
        planner.add(system1);
        auto all = planner.negate(planner.add());

        SUBCASE("directly")
        {
            CHECK_FALSE(planner.repeatWhile(all, trueTwice8));

            expected = {1};
        }

        SUBCASE("indirectly, fail on tag")
        {
            auto tag1 = planner.add();

            CHECK(planner.repeatWhile(tag1, trueTwice8));
            CHECK_FALSE(planner.tag(all, tag1));

            expected = {8, 1, 8, 8};
        }

        SUBCASE("indirectly, fail on repeatWhile")
        {
            auto tag1 = planner.add();

            CHECK(planner.tag(all, tag1));
            CHECK_FALSE(planner.repeatWhile(tag1, trueTwice8));

            expected = {1};
        }
    }

    SUBCASE("tag which refers to all systems")
    {
        auto leaf1 = planner.add(system1);
        planner.add(system2);
        planner.add(system3);
        auto all = planner.negate(planner.add());

        CHECK_FALSE(planner.order(all, leaf1)); // Would require leaf1 running before leaf1.
        CHECK_FALSE(planner.order(leaf1, all)); // Would require leaf1 running after leaf1.
    }

    SUBCASE("parent-child cycles are forbidden")
    {
        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        CHECK(planner.tag(tag1, tag1)); // Does nothing.
        CHECK(planner.tag(tag1, tag2));
        CHECK(planner.tag(tag2, tag3));
        CHECK_FALSE(planner.tag(tag3, tag1));
    }

    SUBCASE("leaf tags cannot be parents")
    {
        auto leaf1 = planner.add(system1);
        auto tag1 = planner.add();

        CHECK_FALSE(planner.tag(tag1, leaf1));

        expected = {1};
    }

    // Build a schedule and run it.
    auto schedule = planner.build();
    schedule.run(registry, context);

    INFO("Result: ", orderToString(world.resource<std::vector<int>>()));
    INFO("Expected: ", orderToString(expected));
    REQUIRE(world.resource<std::vector<int>>() == expected);
}
