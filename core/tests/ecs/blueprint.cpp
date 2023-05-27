#include <doctest/doctest.h>

#include <cubos/core/ecs/blueprint.hpp>

#include "utils.hpp"

using cubos::core::data::Package;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

TEST_CASE("ecs::Blueprint")
{
    Blueprint blueprint{};
    World world{};
    CommandBuffer cmds{world};
    setupWorld(world);

    SUBCASE("create an entity and clear the blueprint immediately")
    {
        // If an entity is created, then the returned identifier must not be null
        auto foo = blueprint.create("foo", IntegerComponent{0});
        CHECK_FALSE(foo.isNull());

        // Searching for an entity with the same name should return the same identifier
        CHECK(blueprint.entity("foo") == foo);

        blueprint.clear();
    }

    // If the blueprint is empty/has been initialized then searching for an entity should always
    // return a null identifier.
    CHECK(blueprint.entity("foo").isNull());

    // Create an entity on the blueprint which references another entity.
    auto bar = blueprint.create("bar");
    blueprint.create("baz", ParentComponent{bar});

    SUBCASE("merge one blueprint into another blueprint")
    {
        // Create another blueprint with one entity.
        Blueprint merged{};
        auto foo = merged.create("foo", IntegerComponent{1});

        // Merge the original blueprint into the new one.
        merged.merge("sub", blueprint);

        // Then the new blueprint has the correct entities.
        CHECK(merged.entity("foo") == foo);
        CHECK_FALSE(merged.entity("sub.bar").isNull());
        CHECK_FALSE(merged.entity("sub.baz").isNull());

        // Spawn the blueprint into the world and get the identifiers of the spawned entities.
        auto spawned = cmds.spawn(merged);
        auto spawnedFoo = spawned.entity("foo");
        auto spawnedBar = spawned.entity("sub.bar");
        auto spawnedBaz = spawned.entity("sub.baz");
        cmds.commit();

        // Package the entities to make sure they were properly spawned.
        auto fooPkg = world.pack(spawnedFoo);
        auto barPkg = world.pack(spawnedBar);
        auto bazPkg = world.pack(spawnedBaz);

        // "foo" has an IntegerComponent with value = 1.
        CHECK(fooPkg.type() == Package::Type::Object);
        CHECK(fooPkg.fields().size() == 1);
        CHECK(fooPkg.field("integer").get<int>() == 1);

        // "bar" has no components.
        CHECK(barPkg.type() == Package::Type::Object);
        CHECK(barPkg.fields().size() == 0);

        // "baz" has a ParentComponent with parent = "bar".
        CHECK(bazPkg.type() == Package::Type::Object);
        CHECK(bazPkg.fields().size() == 1);
        CHECK(bazPkg.field("parent").get<Entity>() == spawnedBar);
    }
}
