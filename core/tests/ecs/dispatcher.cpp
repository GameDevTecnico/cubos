#include <vector>

#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "utils.hpp"

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Dispatcher;
using cubos::core::ecs::System;
using cubos::core::ecs::World;

/// System which pushes N to the order vector.
/// @tparam N
template <int N>
static void pushToOrder(std::vector<int>& order)
{
    order.push_back(N);
}

/// Condition which pushes N to the order vector and always fails.
/// @tparam N
template <int N>
static bool pushToOrderAndFail(std::vector<int>& order)
{
    order.push_back(N);
    return false;
}

/// Condition which pushes N to the order vector and always succeeds.
/// @tparam N
template <int N>
static bool pushToOrderAndSucceed(std::vector<int>& order)
{
    order.push_back(N);
    return true;
}

static bool succeed2Times(int& counter)
{
    counter++;
    if (counter == 4)
    {
        counter++;
        return false;
    }
    return counter <= 8;
}

/// Asserts that the order vector contains the given values in order.
/// @param world The world the order vector is in.
/// @param values The values to check for.
static void assertOrder(World& world, const std::vector<int>& values)
{
    auto order = world.read<std::vector<int>>();
    CHECK(order.get().size() == values.size());
    for (std::size_t i = 0; i < values.size() && i < order.get().size(); ++i)
    {
        CHECK(order.get()[i] == values[i]);
    }
}

/// Compiles the dispatcher chain and calls the systems.
/// @param world The world to call the systems on.
/// @param dispatcher The dispatcher to call.
/// @param cmdBuffer The command buffer to call the systems on.
/// @param values The values to check for.
static void dispatchAndCheck(World& world, Dispatcher& dispatcher, CommandBuffer& cmdBuffer, std::vector<int> values)
{
    dispatcher.compileChain();

    // Run the test twice to ensure the dispatcher is stateless.
    for (int i = 1; i <= 2; ++i)
    {
        INFO("Run ", i);
        world.write<std::vector<int>>().get().clear();
        world.write<int>().get() = 0;
        dispatcher.callSystems(cmdBuffer);
        assertOrder(world, values);
    }
}

TEST_CASE("ecs::Dispatcher")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Dispatcher dispatcher{};
    world.registerResource<std::vector<int>>();
    world.registerResource<int>();

    auto wrapSystem = [&](auto f) { return System<void>::make(world, f); };
    auto wrapCondition = [&](auto f) { return System<bool>::make(world, f); };

    SUBCASE("systems run in the reverse order they were added")
    {
        SUBCASE("without any constraints")
        {
            dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
            dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
            dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        }

        SUBCASE("with constraints on the systems")
        {
            dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
            dispatcher.systemSetAfterTag("2"); // runs after "2"
            dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
            dispatcher.systemAddTag("2");
            dispatcher.systemSetAfterTag("3"); // runs after "3"
            dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
            dispatcher.systemAddTag("3");
        }

        SUBCASE("with constraints on the tags")
        {
            dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
            dispatcher.systemAddTag("1");
            dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
            dispatcher.systemAddTag("2");
            dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
            dispatcher.systemAddTag("3");

            dispatcher.addTag("1");
            dispatcher.tagSetAfterTag("2"); // "1" runs after "2"
            dispatcher.addTag("3");
            dispatcher.tagSetBeforeTag("2"); // "3" runs before "2"
        }

        SUBCASE("with constraints on negative tags")
        {
            dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
            dispatcher.systemAddTag("1");
            dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
            dispatcher.systemAddTag("2");
            dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
            dispatcher.systemAddTag("3");

            dispatcher.addNegativeTag("1");
            dispatcher.tagSetBeforeTag("1"); // anything without "1" runs before "1"
            dispatcher.addNegativeTag("3");
            dispatcher.tagSetAfterTag("3"); // anything without "3" runs after "3"
        }

        dispatchAndCheck(world, dispatcher, cmdBuffer, {3, 2, 1});
    }

    SUBCASE("1 runs first, then 3, and finally 2")
    {
        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddTag("1");
        dispatcher.systemSetBeforeTag("3"); // runs before "3"
        dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
        dispatcher.systemAddTag("2");
        dispatcher.systemSetAfterTag("3"); // runs after "3"
        dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        dispatcher.systemAddTag("3");

        SUBCASE("without any superfluous constraints")
        {
        }

        SUBCASE("with unnecessary constraints")
        {
            dispatcher.addTag("1");
            dispatcher.tagSetBeforeTag("3"); // "1" runs before "3" - redundant
            dispatcher.tagSetBeforeTag("3"); // Repeat
            dispatcher.tagSetBeforeTag("2"); // "1" runs before "2" - redundant

            dispatcher.addTag("2");
            dispatcher.tagSetAfterTag("3"); // "2" runs after "3" - redundant
            dispatcher.tagSetAfterTag("3"); // Repeat
            dispatcher.tagSetAfterTag("1"); // "2" runs after "1" - redundant

            dispatcher.addTag("3");
            dispatcher.tagSetAfterTag("1");  // "3" runs after "1" - redundant
            dispatcher.tagSetAfterTag("1");  // Repeat
            dispatcher.tagSetBeforeTag("2"); // "3" runs before "2" - redundant
            dispatcher.tagSetBeforeTag("2"); // Repeat

            dispatcher.addNegativeTag("1");
            dispatcher.tagSetAfterTag("1"); // "2" and "3" run after "1" - redundant
            dispatcher.tagSetAfterTag("1"); // Repeat

            dispatcher.addNegativeTag("2");
            dispatcher.tagSetBeforeTag("2"); // "1" and "3" run before "2" - redundant
            dispatcher.tagSetBeforeTag("2"); // Repeat
        }

        dispatchAndCheck(world, dispatcher, cmdBuffer, {1, 3, 2});
    }

    SUBCASE("systems run when all of their conditions are fulfilled")
    {
        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddCondition(wrapCondition(pushToOrderAndSucceed<2>));
        dispatcher.systemAddCondition(wrapCondition(pushToOrderAndSucceed<3>));
        dispatcher.addSystem("4", wrapSystem(pushToOrder<4>));
        dispatcher.systemAddTag("4");
        dispatcher.addSystem("5", wrapSystem(pushToOrder<5>));
        dispatcher.systemAddTag("5");

        dispatcher.addTag("4");
        dispatcher.tagInheritTag("45");
        dispatcher.addTag("5");
        dispatcher.tagInheritTag("45");
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndSucceed<6>)); // "45" only runs when "6" succeeds

        // By default, systems run in the reverse order they were added.
        // Conditions run in the order they were added.
        // Since 5 was the last system added, it runs first, after its condition, 6.
        // Then 4 runs, since its condition, 6, succeeded.
        // Then 1 must run after its conditions, 2 and 3, succeed.
        dispatchAndCheck(world, dispatcher, cmdBuffer, {6, 5, 4, 2, 3, 1});
    }

    SUBCASE("systems do not run when their conditions are not met")
    {
        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddTag("1");
        dispatcher.systemAddCondition(wrapCondition(pushToOrderAndSucceed<2>));
        dispatcher.systemAddCondition(wrapCondition(pushToOrderAndFail<3>));
        dispatcher.systemAddCondition(wrapCondition(pushToOrderAndSucceed<4>));
        dispatcher.addSystem("5", wrapSystem(pushToOrder<5>));
        dispatcher.systemAddTag("5");

        dispatcher.addTag("5");
        dispatcher.tagInheritTag("6");
        dispatcher.addTag("6");
        dispatcher.tagInheritTag("7");
        dispatcher.tagInheritTag("7"); // Repeat
        dispatcher.addTag("7");
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndSucceed<8>));
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndFail<9>));
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndFail<10>));

        dispatcher.addSystem("1", wrapSystem(pushToOrder<11>));
        dispatcher.systemSetAfterTag("1");

        // First, 5 tries to run, but its condition 9 fails. The condition 10 is not checked.
        // Then, 1 tries to run, but its condition 3 fails. The condition 4 is not checked.
        // Then, 11 runs, since it has no conditions, and must run after 1 succeeds/is skipped.
        dispatchAndCheck(world, dispatcher, cmdBuffer, {8, 9, 2, 3, 11});
    }

    SUBCASE("constraints are transitive on tags without systems")
    {
        // Tests if #413 has been fixed.
        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemSetBeforeTag("a");
        dispatcher.addTag("a");
        dispatcher.tagSetBeforeTag("b");
        dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        dispatcher.systemAddTag("b");

        // 1 runs before tag "a".
        // tag "a" runs before tag "b".
        // 3 is tagged with "b".
        // Therefore, 1 must run before 3.
        dispatchAndCheck(world, dispatcher, cmdBuffer, {1, 3});
    }

    SUBCASE("repeat while")
    {
        dispatcher.addTag("repeat");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));
        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddGroup("repeat");

        dispatchAndCheck(world, dispatcher, cmdBuffer, {1, 1, 1});
    }

    SUBCASE("repeat while inside repeat while")
    {
        dispatcher.addTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));

        dispatcher.addTag("subtag");
        dispatcher.tagInheritTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));

        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddTag("principal");
        dispatcher.systemAddTag("first");

        dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemSetAfterTag("first");
        dispatcher.systemSetBeforeTag("last");

        dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemAddTag("last");

        dispatchAndCheck(world, dispatcher, cmdBuffer, {1, 2, 3, 2, 3, 1, 2, 3, 2, 3});
    }

    SUBCASE("repeat while inside repeat while with condition in every system")
    {
        dispatcher.addTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndSucceed<5>));

        dispatcher.addTag("subtag");
        dispatcher.tagInheritTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));

        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddTag("principal");
        dispatcher.systemAddTag("first");

        dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemSetAfterTag("first");
        dispatcher.systemSetBeforeTag("last");

        dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemAddTag("last");

        dispatchAndCheck(world, dispatcher, cmdBuffer, {5, 1, 2, 3, 2, 3, 1, 2, 3, 2, 3});
    }

    SUBCASE("repeat while inside repeat while with condition in inner group")
    {
        dispatcher.addTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));

        dispatcher.addTag("subtag");
        dispatcher.tagInheritTag("principal");
        dispatcher.tagRepeatWhile(wrapCondition(succeed2Times));
        dispatcher.tagAddCondition(wrapCondition(pushToOrderAndSucceed<5>));

        dispatcher.addSystem("1", wrapSystem(pushToOrder<1>));
        dispatcher.systemAddTag("principal");
        dispatcher.systemAddTag("first");

        dispatcher.addSystem("2", wrapSystem(pushToOrder<2>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemSetAfterTag("first");
        dispatcher.systemSetBeforeTag("last");

        dispatcher.addSystem("3", wrapSystem(pushToOrder<3>));
        dispatcher.systemAddTag("subtag");
        dispatcher.systemAddTag("last");

        dispatchAndCheck(world, dispatcher, cmdBuffer, {1, 5, 2, 3, 2, 3, 1, 2, 3, 2, 3});
    }
}
