#include <doctest/doctest.h>

#include <cubos/core/ecs/system/commands.hpp>

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
    CHECK(world.has<IntegerComponent>(foo));

    SUBCASE("destroy the entity")
    {
        // Destroy the entity.
        cmds.destroy(foo);
        CHECK(world.isAlive(foo));
        CHECK(world.has<IntegerComponent>(foo));
        cmdBuffer.commit();
        CHECK_FALSE(world.isAlive(foo));
    }

    SUBCASE("remove a component")
    {
        cmds.remove<IntegerComponent>(foo);
        CHECK(world.has<IntegerComponent>(foo));
        cmdBuffer.commit();
        CHECK_FALSE(world.has<IntegerComponent>(foo));
    }

    SUBCASE("add a component")
    {
        auto parent = cmds.create().entity();
        cmds.add(foo, ParentComponent{parent});
        CHECK_FALSE(world.has<ParentComponent>(foo));
        cmdBuffer.commit();
        CHECK(world.has<ParentComponent>(foo));
    }

    SUBCASE("abort creation of entities")
    {
        auto entity = cmds.create().entity();
        CHECK_FALSE(world.isAlive(entity));
        cmdBuffer.abort();
        CHECK_FALSE(world.isAlive(entity));
        cmdBuffer.commit();
        CHECK_FALSE(world.isAlive(entity));
    }
}
