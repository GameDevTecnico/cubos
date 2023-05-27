#include <doctest/doctest.h>

#include <cubos/core/ecs/world.hpp>

#include "utils.hpp"

using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

TEST_CASE("ecs::World")
{
    World world{};
    setupWorld(world);

    // Null identifiers should never be alive.
    CHECK_FALSE(world.isAlive({}));

    // The world should be empty.
    CHECK(world.begin() == world.end());

    // Create an entity.
    auto foo = world.create();
    CHECK_FALSE(foo.isNull());
    CHECK(world.isAlive(foo));

    // It should be the first entity in the world (and last).
    CHECK(*world.begin() == foo);
    CHECK(++world.begin() == world.end());

    // Add components to the entity.
    world.add(foo, IntegerComponent{0}, ParentComponent{});
    CHECK(world.has<IntegerComponent>(foo));
    CHECK(world.has<ParentComponent>(foo));

    // Pack the entity and check if the values are correct.
    auto pkg = world.pack(foo);
    CHECK(pkg.fields().size() == 2);
    CHECK(pkg.field("integer").get<int>() == 0);
    CHECK(pkg.field("parent").get<Entity>().isNull());

    // Change the value of the integer component and remove the parent component.
    pkg.field("integer").set<int>(1);
    pkg.removeField("parent");
    CHECK(world.unpack(foo, pkg));

    // The entity should still have the integer component, but not the parent component.
    CHECK(world.has<IntegerComponent>(foo));
    CHECK_FALSE(world.has<ParentComponent>(foo));

    // Pack the entity again and check if the value was changed.
    pkg = world.pack(foo);
    CHECK(pkg.fields().size() == 1);
    CHECK(pkg.field("integer").get<int>() == 1);

    // Remove the entity.
    world.destroy(foo);
    CHECK_FALSE(world.isAlive(foo));

    // The world should be empty again.
    CHECK(world.begin() == world.end());
}