#include <vector>

#include <doctest/doctest.h>

#include <cubos/core/ecs/system/dispatcher.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "utils.hpp"

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Dispatcher;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;

/// System which pushes N to the order vector.
/// @tparam N
template <int N>
static void pushToOrder(Write<std::vector<int>> order)
{
    order->push_back(N);
}

/// Condition which pushes N to the order vector and always fails.
/// @tparam N
template <int N>
static bool pushToOrderAndFail(Write<std::vector<int>> order)
{
    order->push_back(N);
    return false;
}

/// Condition which pushes N to the order vector and always succeeds.
/// @tparam N
template <int N>
static bool pushToOrderAndSucceed(Write<std::vector<int>> order)
{
    order->push_back(N);
    return true;
}

/// Asserts that the order vector contains the given values in order.
/// @param world The world the order vector is in.
/// @param values The values to check for.
static void assertOrder(World& world, std::vector<int> values)
{
    auto order = world.read<std::vector<int>>();
    REQUIRE(order.get().size() == values.size());
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        CHECK(order.get()[i] == values[i]);
    }
}

/// Compiles the dispatcher chain and calls the systems.
/// @param dispatcher The dispatcher to call.
/// @param world The world to call the systems on.
/// @param cmdBuffer The command buffer to call the systems on.
static void singleDispatch(Dispatcher& dispatcher, World& world, CommandBuffer& cmdBuffer)
{
    dispatcher.compileChain();
    dispatcher.callSystems(world, cmdBuffer);
}

TEST_CASE("ecs::Dispatcher")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Dispatcher dispatcher{};
    world.registerResource<std::vector<int>>();

    SUBCASE("systems run in the reverse order they were added")
    {
        SUBCASE("without any constraints")
        {
            dispatcher.addSystem(pushToOrder<1>);
            dispatcher.addSystem(pushToOrder<2>);
            dispatcher.addSystem(pushToOrder<3>);
        }

        SUBCASE("with constraints on the systems")
        {
            dispatcher.addSystem(pushToOrder<1>);
            dispatcher.systemSetAfterTag("2"); // runs after "2"
            dispatcher.addSystem(pushToOrder<2>);
            dispatcher.systemAddTag("2");
            dispatcher.systemSetAfterTag("3"); // runs after "3"
            dispatcher.addSystem(pushToOrder<3>);
            dispatcher.systemAddTag("3");
        }

        SUBCASE("with constraints on the tags")
        {
            dispatcher.addSystem(pushToOrder<1>);
            dispatcher.systemAddTag("1");
            dispatcher.addSystem(pushToOrder<2>);
            dispatcher.systemAddTag("2");
            dispatcher.addSystem(pushToOrder<3>);
            dispatcher.systemAddTag("3");

            dispatcher.addTag("1");
            dispatcher.tagSetAfterTag("2"); // "1" runs after "2"
            dispatcher.addTag("3");
            dispatcher.tagSetBeforeTag("2"); // "3" runs before "2"
        }

        singleDispatch(dispatcher, world, cmdBuffer);
        assertOrder(world, {3, 2, 1});
    }

    SUBCASE("1 runs first, then 3, and finally 2")
    {
        dispatcher.addSystem(pushToOrder<1>);
        dispatcher.systemAddTag("1");
        dispatcher.systemSetBeforeTag("3"); // runs before "3"
        dispatcher.addSystem(pushToOrder<2>);
        dispatcher.systemAddTag("2");
        dispatcher.systemSetAfterTag("3"); // runs after "3"
        dispatcher.addSystem(pushToOrder<3>);
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
        }

        singleDispatch(dispatcher, world, cmdBuffer);
        assertOrder(world, {1, 3, 2});
    }

    SUBCASE("systems run when all of their conditions are fulfilled")
    {
        dispatcher.addSystem(pushToOrder<1>);
        dispatcher.systemAddCondition(pushToOrderAndSucceed<2>);
        dispatcher.systemAddCondition(pushToOrderAndSucceed<3>);
        dispatcher.addSystem(pushToOrder<4>);
        dispatcher.systemAddTag("4");
        dispatcher.addSystem(pushToOrder<5>);
        dispatcher.systemAddTag("5");

        dispatcher.addTag("4");
        dispatcher.tagInheritTag("45");
        dispatcher.addTag("5");
        dispatcher.tagInheritTag("45");
        dispatcher.tagAddCondition(pushToOrderAndSucceed<6>); // "45" only runs when "6" succeeds

        // By default, systems run in the reverse order they were added.
        // Conditions run in the order they were added.
        // Since 5 was the last system added, it runs first, after its condition, 6.
        // Then 4 runs, since its condition, 6, succeeded.
        // Then 1 must run after its conditions, 2 and 3, succeed.
        singleDispatch(dispatcher, world, cmdBuffer);
        assertOrder(world, {6, 5, 4, 2, 3, 1});
    }

    SUBCASE("systems do not run when their conditions are not met")
    {
        dispatcher.addSystem(pushToOrder<1>);
        dispatcher.systemAddTag("1");
        dispatcher.systemAddCondition(pushToOrderAndSucceed<2>);
        dispatcher.systemAddCondition(pushToOrderAndFail<3>);
        dispatcher.systemAddCondition(pushToOrderAndSucceed<4>);
        dispatcher.addSystem(pushToOrder<5>);
        dispatcher.systemAddTag("5");

        dispatcher.addTag("5");
        dispatcher.tagInheritTag("6");
        dispatcher.addTag("6");
        dispatcher.tagInheritTag("7");
        dispatcher.tagInheritTag("7"); // Repeat
        dispatcher.addTag("7");
        dispatcher.tagAddCondition(pushToOrderAndSucceed<8>);
        dispatcher.tagAddCondition(pushToOrderAndFail<9>);
        dispatcher.tagAddCondition(pushToOrderAndFail<10>);

        dispatcher.addSystem(pushToOrder<11>);
        dispatcher.systemSetAfterTag("1");

        // First, 5 tries to run, but its condition 9 fails. The condition 10 is not checked.
        // Then, 1 tries to run, but its condition 3 fails. The condition 4 is not checked.
        // Then, 11 runs, since it has no conditions, and must run after 1 succeeds/is skipped.
        singleDispatch(dispatcher, world, cmdBuffer);
        assertOrder(world, {8, 9, 2, 3, 11});
    }

    SUBCASE("constraints are transitive on tags without systems")
    {
        // Tests if #413 has been fixed.
        dispatcher.addSystem(pushToOrder<1>);
        dispatcher.systemSetBeforeTag("a");
        dispatcher.addTag("a");
        dispatcher.tagSetBeforeTag("b");
        dispatcher.addSystem(pushToOrder<3>);
        dispatcher.systemAddTag("b");

        // 1 runs before tag "a".
        // tag "a" runs before tag "b".
        // 3 is tagged with "b".
        // Therefore, 1 must run before 3.
        singleDispatch(dispatcher, world, cmdBuffer);
        assertOrder(world, {1, 3});
    }
}
