#include <cstdlib>

#include <doctest/doctest.h>

#include <cubos/core/ecs/observer/observers.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include "utils.hpp"

using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::Entity;
using cubos::core::ecs::SparseRelationTableId;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;

// NOLINTBEGIN(readability-function-size)
TEST_CASE("ecs::World")
{
    World world{};
    const World& constWorld = world;

    setupWorld(world);

    if (rand() % 2 == 0)
    {
        INFO("World was reset");
        world.create(); // Create an entity because why not.
        world.reset();
        setupWorld(world);
    }

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
        CHECK_FALSE(world.hasResource<int>());
        world.registerResource<int>();
        CHECK_FALSE(world.hasResource<int>());

        world.insertResource<int>(42);
        CHECK(world.hasResource<int>());

        world.registerResource<char>();
        world.insertResource<char>('x');

        // Check if the read values are correct.
        CHECK(constWorld.resource<int>() == 42);
        CHECK(world.resource<char>() == 'x');

        // Write new values.
        world.resource<int>() = 43;
        world.resource<char>() = 'y';

        // Check if the read values are correct.
        CHECK(world.resource<int>() == 43);
        CHECK(constWorld.resource<char>() == 'y');
    }

    SUBCASE("resources are correctly destructed when the world is destroyed")
    {
        bool destroyed = false;

        // Create a world with a resource and immediately destroy it.
        {
            World world{};
            world.registerResource<DetectDestructor>();
            world.insertResource(DetectDestructor{&destroyed});
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

    SUBCASE("an entity can have at most one outgoing tree relation")
    {
        // Create three entities.
        auto foo = world.create();
        auto bar = world.create();
        auto baz = world.create();

        // Set up a tree relation from foo to bar.
        world.relate(foo, bar, TreeRelation{});
        CHECK(world.related<TreeRelation>(foo, bar));

        // Set up a tree relation from foo to baz. This should overwrite the previous one.
        world.relate(foo, baz, TreeRelation{});
        CHECK_FALSE(world.related<TreeRelation>(foo, bar));
        CHECK(world.related<TreeRelation>(foo, baz));
    }

    SUBCASE("tree relation depth is managed correctly")
    {
        auto treeRelation = world.types().id(reflect<TreeRelation>());
        SparseRelationTableId tableId = {treeRelation, ArchetypeId::Empty, ArchetypeId::Empty, 0};

        // Create four entities.
        auto foo = world.create();
        auto bar = world.create();
        auto baz = world.create();
        auto qux = world.create();

        // Set up a tree relation from foo to bar.
        world.relate(foo, bar, TreeRelation{});
        REQUIRE(world.tables().sparseRelation().contains(tableId));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.related<TreeRelation>(foo, bar));

        // Set up a tree relation from bar to baz.
        world.relate(bar, baz, TreeRelation{});
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        tableId.depth = 1;
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.related<TreeRelation>(foo, bar));
        REQUIRE(world.related<TreeRelation>(bar, baz));

        // Set up a tree relation from baz to qux.
        world.relate(baz, qux, TreeRelation{});
        tableId.depth = 0;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(baz.index, qux.index));
        tableId.depth = 1;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        tableId.depth = 2;
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.related<TreeRelation>(foo, bar));
        REQUIRE(world.related<TreeRelation>(bar, baz));
        REQUIRE(world.related<TreeRelation>(baz, qux));

        // Remove the relation from bar to baz.
        world.unrelate<TreeRelation>(bar, baz);
        tableId.depth = 0;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(baz.index, qux.index));
        tableId.depth = 1;
        REQUIRE(world.tables().sparseRelation().at(tableId).size() == 0);
        tableId.depth = 2;
        REQUIRE(world.tables().sparseRelation().at(tableId).size() == 0);
        REQUIRE_FALSE(world.related<TreeRelation>(bar, baz));

        // Add back the relation from bar to baz.
        world.relate(bar, baz, TreeRelation{});
        tableId.depth = 0;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(baz.index, qux.index));
        tableId.depth = 1;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        tableId.depth = 2;
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));

        // Remove the relation from baz to qux.
        world.unrelate<TreeRelation>(baz, qux);
        tableId.depth = 0;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(baz.index, qux.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        tableId.depth = 1;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
    }

    SUBCASE("tree relation depth is managed correctly even with destructions")
    {
        auto treeRelation = world.types().id(reflect<TreeRelation>());
        SparseRelationTableId tableId = {treeRelation, ArchetypeId::Empty, ArchetypeId::Empty, 0};

        // Create four entities.
        auto foo = world.create();
        auto bar = world.create();
        auto baz = world.create();

        // Set up a tree relation from foo to bar and from bar to baz.
        world.relate(foo, bar, TreeRelation{});
        world.relate(bar, baz, TreeRelation{});
        tableId.depth = 0;
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        tableId.depth = 1;
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.related<TreeRelation>(foo, bar));
        REQUIRE(world.related<TreeRelation>(bar, baz));

        // Destroy baz.
        world.destroy(baz);
        tableId.depth = 0;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, baz.index));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        tableId.depth = 1;
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        REQUIRE(world.related<TreeRelation>(foo, bar));
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

    SUBCASE("symmetric relations from entity to itself are stored correctly")
    {
        auto foo = world.create();
        world.relate(foo, foo, SymmetricRelation{.value = 1});
        SparseRelationTableId oldTableId{world.types().id("SymmetricRelation"), world.archetype(foo),
                                         world.archetype(foo)};
        REQUIRE(world.tables().sparseRelation().contains(oldTableId));
        REQUIRE(world.tables().sparseRelation().at(oldTableId).contains(foo.index, foo.index));

        // If we change the archetype of foo, the relation should move to the right table.
        world.components(foo).add(IntegerComponent{0});
        SparseRelationTableId newTableId{world.types().id("SymmetricRelation"), world.archetype(foo),
                                         world.archetype(foo)};
        REQUIRE_FALSE(world.tables().sparseRelation().at(oldTableId).contains(foo.index, foo.index));
        REQUIRE(world.tables().sparseRelation().contains(newTableId));
        REQUIRE(world.tables().sparseRelation().at(newTableId).contains(foo.index, foo.index));

        // If we move foo back to the original archetype, the relation should be stored on the original table.
        world.components(foo).remove<IntegerComponent>();
        REQUIRE_FALSE(world.tables().sparseRelation().at(newTableId).contains(foo.index, foo.index));
        REQUIRE(world.tables().sparseRelation().at(oldTableId).contains(foo.index, foo.index));
    }

    SUBCASE("symmetric relation invariant is kept")
    {
        auto foo = world.create();
        auto bar = world.create();
        REQUIRE(foo.index < bar.index);
        REQUIRE(world.archetype(foo) == world.archetype(bar));

        // Since foo < bar, and both have the same archetype, the relation should be stored as (foo, bar).
        world.relate(bar, foo, SymmetricRelation{.value = 1});
        SparseRelationTableId tableId{world.types().id("SymmetricRelation"), world.archetype(foo),
                                      world.archetype(bar)};
        REQUIRE(world.tables().sparseRelation().contains(tableId));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));

        // If we change the archetype of foo, the relation should now be stored as (bar, foo), if
        // foo's archetype is now greater than bar's.
        world.components(foo).add(IntegerComponent{0});
        REQUIRE(world.archetype(foo).inner > world.archetype(bar).inner);
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, foo.index));
        tableId.from = world.archetype(bar);
        tableId.to = world.archetype(foo);
        REQUIRE(world.tables().sparseRelation().contains(tableId));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(bar.index, foo.index));

        // If we move foo back to the original archetype, the relation should be stored as (foo, bar) again.
        world.components(foo).remove<IntegerComponent>();
        REQUIRE(world.archetype(foo) == world.archetype(bar));
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(bar.index, foo.index));
        tableId.from = world.archetype(foo);
        tableId.to = world.archetype(bar);
        REQUIRE(world.tables().sparseRelation().contains(tableId));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));

        // If we change bar's archetype to be greater than foo's, the relation should be still be stored as (foo, bar).
        world.components(bar).add(IntegerComponent{0});
        REQUIRE(world.archetype(bar).inner > world.archetype(foo).inner);
        REQUIRE_FALSE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
        tableId.from = world.archetype(foo);
        tableId.to = world.archetype(bar);
        REQUIRE(world.tables().sparseRelation().contains(tableId));
        REQUIRE(world.tables().sparseRelation().at(tableId).contains(foo.index, bar.index));
    }

    SUBCASE("relation views work correctly")
    {
        auto foo = world.create();
        auto bar = world.create();
        auto fromFoo = world.relationsFrom(foo);
        auto toFoo = static_cast<const World&>(world).relationsTo(foo);

        // No relations for now.
        REQUIRE(fromFoo.begin() == fromFoo.end());
        REQUIRE(toFoo.begin() == toFoo.end());

        // Add a relation from foo to bar.
        REQUIRE_FALSE(fromFoo.has<IntegerRelation>(bar));
        world.relate(foo, bar, IntegerRelation{1});
        REQUIRE(fromFoo.has<IntegerRelation>(bar));
        CHECK(fromFoo.get<IntegerRelation>(bar).value == 1);

        {
            auto it = fromFoo.begin();
            REQUIRE(it != fromFoo.end());
            CHECK(it->type->is<IntegerRelation>());
            CHECK(it->value == &fromFoo.get<IntegerRelation>(bar));
            CHECK(it->entity == bar);
            ++it;
            CHECK(it == fromFoo.end());
        }

        {
            auto it = toFoo.begin();
            CHECK(it == toFoo.end());
        }

        // Add a relation from foo to itself.
        REQUIRE_FALSE(fromFoo.has<IntegerRelation>(foo));
        REQUIRE_FALSE(toFoo.has<IntegerRelation>(foo));
        world.relate(foo, foo, IntegerRelation{2});
        REQUIRE(fromFoo.has<IntegerRelation>(foo));
        REQUIRE(toFoo.has<IntegerRelation>(foo));
        CHECK(fromFoo.get<IntegerRelation>(foo).value == 2);
        CHECK(toFoo.get<IntegerRelation>(foo).value == 2);

        {
            auto it = fromFoo.begin();
            REQUIRE(it != fromFoo.end());
            ++it;
            REQUIRE(it != fromFoo.end());
            CHECK(it->type->is<IntegerRelation>());
            CHECK(it->value == &fromFoo.get<IntegerRelation>(foo));
            CHECK(it->entity == foo);
            ++it;
            CHECK(it == fromFoo.end());
        }

        {
            auto it = toFoo.begin();
            REQUIRE(it != toFoo.end());
            CHECK(it->type->is<IntegerRelation>());
            CHECK(it->value == &toFoo.get<IntegerRelation>(foo));
            CHECK(it->entity == foo);
            ++it;
            CHECK(it == toFoo.end());
        }

        // Add a symmetric relation between foo and bar.
        world.relate(foo, bar, SymmetricRelation{.value = 3});
        REQUIRE(fromFoo.has<SymmetricRelation>(bar));
        REQUIRE(fromFoo.get<SymmetricRelation>(bar).value == 3);
        REQUIRE(toFoo.has<SymmetricRelation>(bar));
        REQUIRE(toFoo.get<SymmetricRelation>(bar).value == 3);
        REQUIRE(&fromFoo.get<SymmetricRelation>(bar) == &toFoo.get<SymmetricRelation>(bar));

        for (auto it = fromFoo.begin(); it != fromFoo.end(); ++it)
        {
            if (it->type->is<SymmetricRelation>())
            {
                CHECK(it->value == &fromFoo.get<SymmetricRelation>(bar));
                CHECK(it->entity == bar);
                break;
            }

            CHECK(it->type->is<IntegerRelation>());
        }

        for (auto it = toFoo.begin(); it != toFoo.end(); ++it)
        {
            if (it->type->is<SymmetricRelation>())
            {
                CHECK(it->value == &toFoo.get<SymmetricRelation>(bar));
                CHECK(it->entity == bar);
                break;
            }

            CHECK(it->type->is<IntegerRelation>());
        }
    }

    SUBCASE("world remove works correctly with remove observers")
    {
        auto foo = world.create();
        auto bar = world.create();

        world.components(foo).add(IntegerComponent{0});
        auto columnId = cubos::core::ecs::ColumnId::make(world.types().id<IntegerComponent>());
        world.observers().hookOnRemove(columnId, cubos::core::ecs::System<void>::make(world, [foo, bar](World& w) {
                                           w.components(foo).add(ParentComponent{bar});
                                       }));
        // Removing IntegerComponent triggers observer, which alters the entity
        world.components(foo).remove<IntegerComponent>();
        CHECK_FALSE(world.components(foo).has<IntegerComponent>());
        CHECK(world.components(foo).has<ParentComponent>());
    }

    SUBCASE("world destroy works correctly with remove observers")
    {
        auto foo = world.create();
        auto bar = world.create();

        world.components(foo).add(IntegerComponent{0});
        auto columnId = cubos::core::ecs::ColumnId::make(world.types().id<IntegerComponent>());
        world.observers().hookOnRemove(columnId, cubos::core::ecs::System<void>::make(world, [foo, bar](World& w) {
                                           w.components(foo).add(ParentComponent{bar});
                                           w.components(bar).add(ParentComponent{foo});
                                       }));
        // Destroying foo triggers observer, which alters the entity
        world.destroy(foo);
        CHECK_FALSE(world.isAlive(foo));
        CHECK(world.components(bar).has<ParentComponent>());
    }
}
// NOLINTEND(readability-function-size)
