#include <doctest/doctest.h>

#include <cubos/core/ecs/world.hpp>

#include "utils.hpp"

using cubos::core::data::Package;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

TEST_CASE("ecs::World")
{
    World world{};
    setupWorld(world);

    SUBCASE("create an entity and then destroy it")
    {
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

        // Remove the entity.
        world.destroy(foo);
        CHECK_FALSE(world.isAlive(foo));

        // The world should be empty again.
        CHECK(world.begin() == world.end());
    }

    SUBCASE("add and remove components")
    {
        // Create an entity with an integer component.
        auto foo = world.create(IntegerComponent{0});
        CHECK(world.has<IntegerComponent>(foo));
        CHECK_FALSE(world.has<ParentComponent>(foo));

        // Add a parent component.
        world.add(foo, ParentComponent{});
        CHECK(world.has<IntegerComponent>(foo));
        CHECK(world.has<ParentComponent>(foo));

        // Remove the integer component.
        world.remove<IntegerComponent>(foo);
        CHECK_FALSE(world.has<IntegerComponent>(foo));
        CHECK(world.has<ParentComponent>(foo));
    }

    SUBCASE("add and remove components through pack and unpack")
    {
        // Create an entity.
        auto foo = world.create();

        // Add an integer component.
        auto pkg = world.pack(foo);
        pkg.fields().emplace_back("integer", Package::from(1));
        CHECK(world.unpack(foo, pkg));
        CHECK(world.has<IntegerComponent>(foo));
        CHECK_FALSE(world.has<ParentComponent>(foo));

        // Check if the component was added.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 1);
        CHECK(pkg.field("integer").get<int>() == 1);

        // Remove the integer component and add a parent component.
        pkg.removeField("integer");
        pkg.fields().emplace_back("parent", Package::from(Entity{}));
        CHECK(world.unpack(foo, pkg));
        CHECK_FALSE(world.has<IntegerComponent>(foo));
        CHECK(world.has<ParentComponent>(foo));

        // Check if the component was added.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 1);
        CHECK(pkg.field("parent").get<Entity>().isNull());
    }

    SUBCASE("change a component through pack and unpack")
    {
        // Create an entity which has an integer component and a parent component.
        auto bar = world.create();
        auto foo = world.create(IntegerComponent{0}, ParentComponent{bar});
        CHECK(world.has<IntegerComponent>(foo));
        CHECK(world.has<ParentComponent>(foo));

        // Change the value of the integer component.
        auto pkg = world.pack(foo);
        pkg.field("integer").set<int>(1);
        CHECK(world.unpack(foo, pkg));
        CHECK(world.has<IntegerComponent>(foo));
        CHECK(world.has<ParentComponent>(foo));

        // Check if the value was changed.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 2);
        CHECK(pkg.field("integer").get<int>() == 1);
        CHECK(pkg.field("parent").get<Entity>() == bar);
    }
}
