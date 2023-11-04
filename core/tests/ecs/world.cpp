#include <doctest/doctest.h>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/type.hpp>

#include "utils.hpp"

using cubos::core::data::old::Package;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

TEST_CASE("ecs::World")
{
    World world{};
    const World& constWorld = world;

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

        // It shouldn't have any components.
        auto components = world.components(foo);
        CHECK(components.begin() == components.end());
        auto constComponents = constWorld.components(foo);
        CHECK(constComponents.begin() == constComponents.end());

        // Remove the entity.
        world.destroy(foo);
        CHECK_FALSE(world.isAlive(foo));

        // The world should be empty again.
        CHECK(world.begin() == world.end());
    }

    SUBCASE("add and remove components")
    {
        bool destroyed = false;

        // Create an entity with a detect destructor component.
        auto foo = world.create();
        world.components(foo).add(DetectDestructorComponent{{&destroyed}});
        CHECK(world.components(foo).has<DetectDestructorComponent>());
        CHECK_FALSE(world.components(foo).has<ParentComponent>());
        CHECK_FALSE(destroyed);

        // Iterating gives us a single component
        auto components = world.components(foo);
        CHECK(components.begin() != components.end());
        CHECK(++components.begin() == components.end());
        CHECK(components.begin()->type->is<DetectDestructorComponent>());

        auto constComponents = constWorld.components(foo);
        CHECK(constComponents.begin() != constComponents.end());
        CHECK(++constComponents.begin() == constComponents.end());
        CHECK(constComponents.begin()->type->is<DetectDestructorComponent>());

        // Add a parent component.
        world.components(foo).add(ParentComponent{});
        CHECK(world.components(foo).has<DetectDestructorComponent>());
        CHECK(world.components(foo).has<ParentComponent>());
        CHECK_FALSE(destroyed);

        // Remove the detect destructor component.
        world.components(foo).remove<DetectDestructorComponent>();
        CHECK_FALSE(world.components(foo).has<DetectDestructorComponent>());
        CHECK(constWorld.components(foo).has<ParentComponent>());
        CHECK(destroyed);
    }

    SUBCASE("add and remove components through pack and unpack")
    {
        // Create an entity.
        auto foo = world.create();

        // Add an integer component.
        auto pkg = world.pack(foo);
        pkg.fields().emplace_back("IntegerComponent", Package::from(1));
        CHECK(world.unpack(foo, pkg));
        CHECK(world.components(foo).has<IntegerComponent>());
        CHECK_FALSE(world.components(foo).has<ParentComponent>());

        // Check if the component was added.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 1);
        CHECK(pkg.field("IntegerComponent").get<int>() == 1);

        // Remove the integer component and add a parent component.
        pkg.removeField("IntegerComponent");
        pkg.fields().emplace_back("ParentComponent", Package::from(Entity{}));
        CHECK(world.unpack(foo, pkg));
        CHECK_FALSE(world.components(foo).has<IntegerComponent>());
        CHECK(world.components(foo).has<ParentComponent>());

        // Check if the component was added.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 1);
        CHECK(pkg.field("ParentComponent").get<Entity>().isNull());
    }

    SUBCASE("change a component through pack and unpack")
    {
        // Create an entity which has an integer component and a parent component.
        auto bar = world.create();
        auto foo = world.create();
        world.components(foo).add(IntegerComponent{0}).add(ParentComponent{bar});
        CHECK(world.components(foo).has<IntegerComponent>());
        CHECK(world.components(foo).has<ParentComponent>());

        // Change the value of the integer component.
        auto pkg = world.pack(foo);
        pkg.field("IntegerComponent").set<int>(1);
        CHECK(world.unpack(foo, pkg));
        CHECK(world.components(foo).has<IntegerComponent>());
        CHECK(world.components(foo).has<ParentComponent>());

        // Check if the value was changed.
        pkg = world.pack(foo);
        CHECK(pkg.fields().size() == 2);
        CHECK(pkg.field("IntegerComponent").get<int>() == 1);
        CHECK(pkg.field("ParentComponent").get<Entity>() == bar);
    }

    SUBCASE("components are correctly destructed when their entity is destroyed")
    {
        bool destroyed = false;

        // Create an entity and add a detect destructor component.
        auto foo = world.create();
        world.components(foo).add(DetectDestructorComponent{{&destroyed}});
        CHECK_FALSE(destroyed);

        // Destroy the entity.
        world.destroy(foo);
        CHECK(destroyed);
    }

    SUBCASE("components are correctly destructed when the world is destroyed")
    {
        bool destroyed = false;

        // Create a world with an entity and a component and immediately destroy it.
        {
            World world{};
            setupWorld(world);
            auto foo = world.create();
            world.components(foo).add(DetectDestructorComponent{{&destroyed}});
            CHECK_FALSE(destroyed);
        }

        CHECK(destroyed);
    }

    SUBCASE("read and write resources")
    {
        // Register some resources.
        world.registerResource<int>(42);
        world.registerResource<char>('x');

        // Check if the read values are correct.
        CHECK(world.read<int>().get() == 42);
        CHECK(world.read<char>().get() == 'x');

        // Write new values.
        world.write<int>().get() = 43;
        world.write<char>().get() = 'y';

        // Check if the read values are correct.
        CHECK(world.read<int>().get() == 43);
        CHECK(world.read<char>().get() == 'y');
    }

    SUBCASE("resources are correctly destructed when the world is destroyed")
    {
        bool destroyed = false;

        // Create a world with a resource and immediately destroy it.
        {
            World world{};
            world.registerResource<DetectDestructor>(&destroyed);
            CHECK_FALSE(destroyed);
        }

        CHECK(destroyed);
    }
}
