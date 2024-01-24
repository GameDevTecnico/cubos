#include <doctest/doctest.h>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/type.hpp>

#include "utils.hpp"

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

        // Create an entity.
        auto foo = world.create();
        CHECK_FALSE(foo.isNull());
        CHECK(world.isAlive(foo));

        // It shouldn't have any components.
        auto components = world.components(foo);
        CHECK(components.begin() == components.end());
        auto constComponents = constWorld.components(foo);
        CHECK(constComponents.begin() == constComponents.end());

        // Remove the entity.
        world.destroy(foo);
        CHECK_FALSE(world.isAlive(foo));
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

    SUBCASE("add and remove relations")
    {
        // Create three entities.
        auto foo = world.create();
        auto bar = world.create();
        auto baz = world.create();

        bool fooBarFlag = false;
        bool barBazFlag = false;
        bool fooBazFlag = false;
        bool fooFooFlag = false;

        // Relate foo to bar.
        REQUIRE_FALSE(world.related<DetectDestructorRelation>(foo, bar));
        world.relate(foo, bar, DetectDestructorRelation{{&fooBarFlag}});
        REQUIRE(world.related<DetectDestructorRelation>(foo, bar));

        // Relate bar to baz.
        REQUIRE_FALSE(world.related<DetectDestructorRelation>(bar, baz));
        world.relate(bar, baz, DetectDestructorRelation{{&barBazFlag}});
        REQUIRE(world.related<DetectDestructorRelation>(bar, baz));

        // Relate foo to baz.
        REQUIRE_FALSE(world.related<DetectDestructorRelation>(foo, baz));
        world.relate(foo, baz, DetectDestructorRelation{{&fooBazFlag}});
        REQUIRE(world.related<DetectDestructorRelation>(foo, baz));

        // Relate foo to foo.
        REQUIRE_FALSE(world.related<DetectDestructorRelation>(foo, foo));
        world.relate(foo, foo, DetectDestructorRelation{{&fooFooFlag}});
        REQUIRE(world.related<DetectDestructorRelation>(foo, foo));

        // Nothing should have been destroyed yet.
        CHECK_FALSE(fooBarFlag);
        CHECK_FALSE(barBazFlag);
        CHECK_FALSE(fooBazFlag);
        CHECK_FALSE(fooFooFlag);

        // Remove the relation between foo and bar and add it again.
        world.unrelate<DetectDestructorRelation>(foo, bar);
        CHECK(fooBarFlag);
        CHECK_FALSE(barBazFlag);
        CHECK_FALSE(fooBazFlag);
        CHECK_FALSE(fooFooFlag);
        fooBarFlag = false;
        world.relate(foo, bar, DetectDestructorRelation{{&fooBarFlag}});

        // Destroy bar.
        world.destroy(bar);
        CHECK(fooBarFlag);
        CHECK(barBazFlag);
        CHECK_FALSE(fooBazFlag);
        CHECK_FALSE(fooFooFlag);

        // Destroy foo.
        world.destroy(foo);
        CHECK(fooBazFlag);
        CHECK(fooFooFlag);
    }

    SUBCASE("relations are correctly destructed when the world is destroyed")
    {
        bool destroyed = false;

        // Create a world with an entity and a relation and immediately destroy it.
        {
            World world{};
            setupWorld(world);
            auto foo = world.create();
            world.relate(foo, foo, DetectDestructorRelation{{&destroyed}});
            CHECK_FALSE(destroyed);
        }

        CHECK(destroyed);
    }

    SUBCASE("relations are moved correctly between archetypes")
    {
        bool destroyed = false;
        auto foo = world.create();
        auto bar = world.create();

        // Add a relation between foo and bar.
        REQUIRE_FALSE(world.related<DetectDestructorRelation>(foo, bar));
        world.relate(foo, bar, DetectDestructorRelation{{&destroyed}});
        REQUIRE(world.related<DetectDestructorRelation>(foo, bar));
        REQUIRE_FALSE(destroyed);

        // Change the archetype of foo. The entities should still be related.
        world.components(foo).add(IntegerComponent{0});
        REQUIRE(world.related<DetectDestructorRelation>(foo, bar));
        REQUIRE_FALSE(destroyed);

        // Change the archetype of bar. The entities should still be related.
        world.components(bar).add(IntegerComponent{0});
        REQUIRE(world.related<DetectDestructorRelation>(foo, bar));
        REQUIRE_FALSE(destroyed);

        // Change the archetypes once again.
        world.components(foo).remove<IntegerComponent>();
        world.components(bar).remove<IntegerComponent>();
        REQUIRE(world.related<DetectDestructorRelation>(foo, bar));
        REQUIRE_FALSE(destroyed);

        // Destroy foo. The relation should be destroyed.
        world.destroy(foo);
        REQUIRE(destroyed);
    }
}
