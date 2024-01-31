#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/arguments/commands.hpp>

#include "utils.hpp"

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::World;

TEST_CASE("ecs::Commands")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Commands cmds{cmdBuffer};
    setupWorld(world);

    // Create an entity.
    auto foo = cmds.create().add(IntegerComponent{0}).entity();
    CHECK_FALSE(foo.isNull());
    CHECK_FALSE(world.isAlive(foo)); // Still hasn't been committed.
    cmdBuffer.commit();
    CHECK(world.isAlive(foo)); // Now it has been committed.
    CHECK(world.components(foo).has<IntegerComponent>());

    SUBCASE("destroy the entity")
    {
        // Destroy the entity.
        cmds.destroy(foo);
        CHECK(world.isAlive(foo));
        CHECK(world.components(foo).has<IntegerComponent>());
        cmdBuffer.commit();
        CHECK_FALSE(world.isAlive(foo));
    }

    SUBCASE("remove a component")
    {
        cmds.remove<IntegerComponent>(foo);
        CHECK(world.components(foo).has<IntegerComponent>());
        cmdBuffer.commit();
        CHECK_FALSE(world.components(foo).has<IntegerComponent>());
    }

    SUBCASE("add a component")
    {
        auto parent = cmds.create().entity();
        cmds.add(foo, ParentComponent{parent});
        CHECK_FALSE(world.components(foo).has<ParentComponent>());
        cmdBuffer.commit();
        CHECK(world.components(foo).has<ParentComponent>());
    }
}
