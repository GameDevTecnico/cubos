#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/planner.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include "utils.hpp"

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

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
    world.insertResource(AnyValue::customConstruct<std::vector<int>>());
    world.insertResource(AnyValue::customConstruct<std::map<int, int>>());

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
    bool fails{false};

    SUBCASE("simple systems run in default order")
    {
        planner.add("1", system1);
        planner.add("2", system2);
        planner.add("3", system3);

        expected = {3, 2, 1};
    }

    SUBCASE("same system can be added multiple times")
    {
        planner.add("1a", system1);
        planner.add("1b", system1);

        expected = {1, 1};
    }

    SUBCASE("ordering can be specified directly on leaf tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto leaf3 = planner.add("3", system3);

        SUBCASE("3 1 2")
        {
            SUBCASE("simple")
            {
                planner.order(leaf1, leaf2);
            }

            SUBCASE("redundant constraint")
            {
                planner.order(leaf1, leaf2);
                planner.order(leaf1, leaf2);
            }

            SUBCASE("forced")
            {
                planner.order(leaf3, leaf1);
                planner.order(leaf1, leaf2);
            }

            expected = {3, 1, 2};
        }

        SUBCASE("1 2 3")
        {
            SUBCASE("forced")
            {
                planner.order(leaf1, leaf2);
                planner.order(leaf2, leaf3);
            }

            SUBCASE("redundant constraint")
            {
                planner.order(leaf1, leaf2);
                planner.order(leaf2, leaf3);
                planner.order(leaf1, leaf3);
            }

            expected = {1, 2, 3};
        }
    }

    SUBCASE("cyclic ordering constraints are detected")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);

        SUBCASE("directly")
        {
            planner.order(leaf1, leaf2);
            planner.order(leaf2, leaf1);
        }

        SUBCASE("indirectly")
        {
            auto leaf3 = planner.add("3", system3);

            planner.order(leaf1, leaf2);
            planner.order(leaf2, leaf3);
            planner.order(leaf3, leaf1);
        }

        fails = true;
    }

    SUBCASE("ordering is transitive across one tag")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto tag1 = planner.add();

        planner.order(leaf1, tag1);
        planner.order(tag1, leaf2);

        expected = {1, 2};
    }

    SUBCASE("ordering is transitive across multiple tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        planner.order(leaf1, tag1);
        planner.order(tag1, tag2);
        planner.order(tag2, tag3);
        planner.order(tag3, leaf2);

        expected = {1, 2};
    }

    SUBCASE("ordering can be specified through tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        CHECK(planner.tag(leaf1, tag1));
        CHECK(planner.tag(leaf2, tag3));

        planner.order(tag1, tag2);
        planner.order(tag2, tag3);

        expected = {1, 2};
    }

    SUBCASE("cycles are detected for transitive ordering across tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto tag1 = planner.add();

        planner.order(leaf1, tag1);
        planner.order(tag1, leaf2);
        planner.order(leaf2, leaf1);

        fails = true;
    }

    SUBCASE("conditions run with same constraints as their tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        planner.add("3", system3);

        planner.order(leaf1, leaf2);
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
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto leaf3 = planner.add("3", system3);

        auto tag1 = planner.add();
        planner.tag(leaf1, tag1);
        planner.tag(leaf2, tag1);

        auto tag2 = planner.add();
        planner.tag(leaf3, tag2);

        planner.order(tag1, tag2);

        SUBCASE("no internal ordering")
        {
            expected = {2, 1, 3};
        }

        SUBCASE("with internal ordering")
        {
            planner.order(leaf1, leaf2);

            expected = {1, 2, 3};
        }
    }

    SUBCASE("conditions affect entire tags")
    {
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);
        auto leaf3 = planner.add("3", system3);

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
        auto leaf1 = planner.add("1", system1);
        auto leaf2 = planner.add("2", system2);

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

        CHECK(planner.repeatWhile(tag2, trueTwice8));
        CHECK(planner.tag(tag3, tag1));
        CHECK(planner.tag(tag3, tag2));

        fails = true;
    }

    SUBCASE("parent-child cycles are forbidden")
    {
        auto tag1 = planner.add();
        auto tag2 = planner.add();
        auto tag3 = planner.add();

        CHECK(planner.tag(tag1, tag2));
        CHECK(planner.tag(tag2, tag3));
        CHECK(planner.tag(tag3, tag1));

        fails = true;
    }

    SUBCASE("leaf tags cannot be parents")
    {
        auto leaf1 = planner.add("1", system1);
        auto tag1 = planner.add();

        CHECK_FALSE(planner.tag(tag1, leaf1));

        expected = {1};
    }

    // Build a schedule and run it.
    auto schedule = planner.build();
    if (fails)
    {
        REQUIRE(!schedule.contains());
    }
    else
    {
        REQUIRE(schedule.contains());
        schedule->run(registry, context);

        INFO("Result: ", orderToString(world.resource<std::vector<int>>()));
        INFO("Expected: ", orderToString(expected));
        REQUIRE(world.resource<std::vector<int>>() == expected);
    }
}
