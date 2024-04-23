#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/schedule.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include "utils.hpp"

static Schedule::NodeId validate(Opt<Schedule::NodeId> nodeId)
{
    REQUIRE(nodeId.contains());
    return nodeId.value();
}

TEST_CASE("ecs::Schedule")
{
    // Prepare world and context.
    World world{};
    CommandBuffer cmdBuffer{world};
    SystemContext context{.cmdBuffer = cmdBuffer};

    // Register a vector which will store the system order, and a map which will be used to store whether conditions
    // should keep returning true.
    world.registerResource<std::vector<int>>();
    world.registerResource<std::map<int, int>>();

    // Create some systems and conditions.
    SystemRegistry registry{};
    auto system1 = registry.add("system1", makeSystem(world, pushToOrder<1>));
    auto system2 = registry.add("system2", makeSystem(world, pushToOrder<2>));
    auto system3 = registry.add("system3", makeSystem(world, pushToOrder<3>));
    auto true5 = registry.add("true5", makeCondition(world, pushToOrderAndReturn<5, true>));
    auto true6 = registry.add("true6", makeCondition(world, pushToOrderAndReturn<6, true>));
    auto false7 = registry.add("false7", makeCondition(world, pushToOrderAndReturn<7, false>));
    auto trueOnce9 = registry.add("trueOnce9", makeCondition(world, pushToOrderAndTrueNTimes<9, 1>));
    auto trueTwice10 = registry.add("trueTwice10", makeCondition(world, pushToOrderAndTrueNTimes<10, 2>));
    auto trueThrice11 = registry.add("trueThrice11", makeCondition(world, pushToOrderAndTrueNTimes<11, 3>));

    Schedule schedule{};
    std::vector<int> expected{};

    SUBCASE("with systems only")
    {
        auto node1 = validate(schedule.system(system1));
        auto node2 = validate(schedule.system(system2));
        auto node3 = validate(schedule.system(system3));

        SUBCASE("by default systems run in the reverse order of their addition")
        {
            expected = {3, 2, 1};
        }

        SUBCASE("if a subset of systems has their order specified, the remaining systems keep the default order")
        {
            SUBCASE("1 and 2")
            {
                CHECK(schedule.order(node1, node2));

                expected = {3, 1, 2};
            }

            SUBCASE("1 and 3")
            {
                CHECK(schedule.order(node1, node3));

                expected = {2, 1, 3};
            }

            SUBCASE("2 and 3")
            {
                CHECK(schedule.order(node2, node3));

                expected = {2, 1, 3};
            }
        }

        SUBCASE("redundant orders do not affect the end result")
        {
            CHECK(schedule.order(node1, node2));
            CHECK(schedule.order(node2, node3));

            SUBCASE("not redundant")
            {
            }

            SUBCASE("redundant through duplicated orderings")
            {
                CHECK(schedule.order(node1, node2));
                CHECK(schedule.order(node2, node3));
            }

            SUBCASE("redundant through transitivity")
            {
                CHECK(schedule.order(node1, node3));
            }

            expected = {1, 2, 3};
        }

        SUBCASE("cycles are detected, prevented and recoverable")
        {
            CHECK(schedule.order(node1, node2));
            CHECK(schedule.order(node2, node3));
            CHECK_FALSE(schedule.order(node3, node1));

            expected = {1, 2, 3};
        }
    }

    SUBCASE("with systems and conditions")
    {
        auto node1 = validate(schedule.system(system1));
        auto node2 = validate(schedule.system(system2));
        auto node3 = validate(schedule.system(system3));

        SUBCASE("using a system as a condition fails and causes no effect")
        {
            CHECK_FALSE(schedule.onlyIf(node1, node2));

            expected = {3, 2, 1};
        }

        SUBCASE("condition returns true")
        {
            auto node5 = validate(schedule.condition(true5));

            CHECK(schedule.onlyIf(node2, node5));

            SUBCASE("single affected node")
            {
                expected = {5, 3, 1, 2};
            }

            SUBCASE("two affected nodes")
            {
                CHECK(schedule.onlyIf(node3, node5));

                expected = {5, 1, 3, 2};
            }
        }

        SUBCASE("condition returns false")
        {
            auto node7 = validate(schedule.condition(false7));

            CHECK(schedule.onlyIf(node2, node7));

            SUBCASE("single affected node")
            {
                expected = {7, 3, 1};
            }

            SUBCASE("two affected nodes")
            {
                CHECK(schedule.onlyIf(node1, node7));

                expected = {7, 3};
            }
        }

        SUBCASE("condition which depends on other condition")
        {
            CHECK(schedule.order(node1, node2));
            CHECK(schedule.order(node2, node3));

            SUBCASE("both return true")
            {
                auto node5 = validate(schedule.condition(true5));
                auto node6 = validate(schedule.condition(true6));

                schedule.onlyIf(node1, node5);
                schedule.onlyIf(node5, node6);

                expected = {6, 5, 1, 2, 3};
            }

            SUBCASE("outer returns false")
            {
                auto node5 = validate(schedule.condition(true5));
                auto node7 = validate(schedule.condition(false7));

                schedule.onlyIf(node1, node5);
                schedule.onlyIf(node5, node7);

                expected = {7, 2, 3};
            }

            SUBCASE("inner returns false")
            {
                auto node5 = validate(schedule.condition(true5));
                auto node7 = validate(schedule.condition(false7));

                schedule.onlyIf(node1, node7);
                schedule.onlyIf(node7, node5);

                expected = {5, 7, 2, 3};
            }
        }

        SUBCASE("systems ordered in relation to conditioned system runs even the condition is false")
        {
            auto node7 = validate(schedule.condition(false7));

            CHECK(schedule.order(node1, node7));
            CHECK(schedule.onlyIf(node2, node7));
            CHECK(schedule.order(node2, node3));

            expected = {1, 7, 3};
        }
    }

    SUBCASE("with repeats")
    {
        SUBCASE("empty repeat")
        {
            validate(schedule.repeat(trueTwice10));

            expected = {10, 10, 10};
        }

        SUBCASE("two nodes run twice")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            validate(schedule.system(system1, node10));
            validate(schedule.system(system2, node10));

            expected = {10, 2, 1, 10, 2, 1, 10};
        }

        SUBCASE("two nodes run six times due to nested repeats")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node11 = validate(schedule.repeat(trueThrice11, node10));
            validate(schedule.system(system1, node11));
            validate(schedule.system(system2, node11));

            expected = {10, 11, 2, 1, 11, 2, 1, 11, 2, 1, 11, 10, 11, 2, 1, 11, 2, 1, 11, 2, 1, 11, 10};
        }

        SUBCASE("system within repeat order with system within repeat")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node1 = validate(schedule.system(system1, node10));
            auto node2 = validate(schedule.system(system2, node10));

            CHECK(schedule.order(node1, node2));

            expected = {10, 1, 2, 10, 1, 2, 10};
        }

        SUBCASE("system within repeat ordered with system outside repeat")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node1 = validate(schedule.system(system1, node10));
            auto node2 = validate(schedule.system(system2, node10));
            auto node3 = validate(schedule.system(system3));

            CHECK(schedule.order(node3, node1));

            // Would force node10 to run both before and after node3
            CHECK_FALSE(schedule.order(node2, node3));

            expected = {3, 10, 2, 1, 10, 2, 1, 10};
        }

        SUBCASE("contradictory orderings must not present within same repeat")
        {
            auto node9 = validate(schedule.repeat(trueOnce9));
            auto node1 = validate(schedule.system(system1, node9));
            auto node2 = validate(schedule.system(system2, node9));
            auto node3 = validate(schedule.system(system3));

            CHECK(schedule.order(node1, node3));

            // Would force node9 to run both before and after node3
            CHECK_FALSE(schedule.order(node3, node2));

            expected = {9, 2, 1, 9, 3};
        }

        SUBCASE("ordering within repeat")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node1 = validate(schedule.system(system1, node10));
            auto node2 = validate(schedule.system(system2, node10));
            validate(schedule.system(system3, node10));

            CHECK(schedule.order(node1, node2));
            CHECK_FALSE(schedule.order(node2, node1));

            expected = {10, 3, 1, 2, 10, 3, 1, 2, 10};
        }

        SUBCASE("repeating condition")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node9 = validate(schedule.condition(trueOnce9, node10));
            auto node1 = validate(schedule.system(system1, node10));

            CHECK(schedule.onlyIf(node1, node9));

            expected = {10, 9, 1, 10, 9, 10};
        }

        SUBCASE("system node cannot be part of a system node")
        {
            auto node1 = validate(schedule.system(system1));
            CHECK_FALSE(schedule.system(system2, node1));

            expected = {1};
        }

        SUBCASE("system node cannot be part of a condition node")
        {
            auto node7 = validate(schedule.condition(false7));
            CHECK_FALSE(schedule.system(system2, node7));

            expected = {7};
        }

        SUBCASE("repeat cannot be used as a condition")
        {
            auto node1 = validate(schedule.system(system1));
            auto node9 = validate(schedule.repeat(trueOnce9));

            CHECK_FALSE(schedule.onlyIf(node1, node9));

            expected = {9, 1, 9};
        }

        SUBCASE("condition outside repeat affecting system within repeat")
        {
            auto node7 = validate(schedule.condition(false7));
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node1 = validate(schedule.system(system1, node10));
            CHECK(schedule.onlyIf(node1, node7));

            expected = {10, 7, 10, 10};
        }

        SUBCASE("non-repeating condition and repeating condition affecting repeating systems")
        {
            auto node5 = validate(schedule.condition(true5));
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node6 = validate(schedule.condition(true6, node10));
            auto node11 = validate(schedule.repeat(trueThrice11, node10));
            auto node1 = validate(schedule.system(system1, node11));
            auto node2 = validate(schedule.system(system2, node11));

            CHECK(schedule.order(node1, node2));
            CHECK(schedule.onlyIf(node1, node6));
            CHECK(schedule.onlyIf(node2, node5));

            expected = {10, 5, 11, 6, 1, 2, 11, 1, 2, 11, 1, 2, 11, 10, 11, 6, 1, 2, 11, 1, 2, 11, 1, 2, 11, 10};
        }

        SUBCASE("systems must not depend on conditions inside different repeats")
        {
            auto node10 = validate(schedule.repeat(trueTwice10));
            auto node7 = validate(schedule.condition(false7, node10));
            auto node1 = validate(schedule.system(system1));

            CHECK_FALSE(schedule.onlyIf(node1, node7));

            expected = {1, 10, 7, 10, 7, 10};
        }

        SUBCASE("two repeats side by side")
        {
            auto node9 = validate(schedule.repeat(trueOnce9));
            auto node10 = validate(schedule.repeat(trueTwice10));

            auto node1 = validate(schedule.system(system1, node9));
            auto node2 = validate(schedule.system(system2, node10));
            auto node3 = validate(schedule.system(system3, node10));

            SUBCASE("default ordering")
            {
                expected = {10, 9, 3, 2, 1, 10, 9, 3, 2, 10};
            }

            SUBCASE("ordering between repeats")
            {
                CHECK(schedule.order(node1, node2));
                CHECK_FALSE(schedule.order(node3, node1));

                expected = {9, 1, 9, 10, 3, 2, 10, 3, 2, 10};
            }
        }
    }

    // Run the schedule three times, and make sure that on each time the result is the expected one.
    // We repeat it to make sure any state is being properly reset.
    for (int i = 0; i < 3; ++i)
    {
        world.insertResource(std::vector<int>{});
        world.insertResource(std::map<int, int>{});
        schedule.run(registry, context);

        INFO("Iteration: ", i);
        INFO("Result: ", orderToString(world.resource<std::vector<int>>()));
        INFO("Expected: ", orderToString(expected));
        REQUIRE(world.resource<std::vector<int>>() == expected);
    }

    // After clearing the schedule, no nodes should run.
    world.insertResource(std::vector<int>{});
    schedule.clear();
    schedule.run(registry, context);
    CHECK(world.resource<std::vector<int>>().empty());
}
