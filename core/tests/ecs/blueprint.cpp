#include <doctest/doctest.h>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/system/commands.hpp>

#include "utils.hpp"

using cubos::core::data::old::Package;
using cubos::core::data::old::Unpackager;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Registry;
using cubos::core::ecs::World;

TEST_CASE("ecs::Blueprint")
{
    Blueprint blueprint{};
    World world{};
    CommandBuffer cmdBuffer{world};
    Commands cmds{cmdBuffer};
    setupWorld(world);

    // Check if entity name validity is correct
    CHECK(blueprint.validEntityName("foo"));
    CHECK(blueprint.validEntityName("foo-bar"));
    CHECK_FALSE(blueprint.validEntityName("Foo"));
    CHECK_FALSE(blueprint.validEntityName("foo bar"));
    CHECK_FALSE(blueprint.validEntityName("foo.bar"));

    SUBCASE("create an entity and clear the blueprint immediately")
    {
        // If an entity is created, then the returned identifier must not be null
        auto foo = blueprint.create("foo");
        CHECK_FALSE(foo.isNull());
        blueprint.add(foo, IntegerComponent{0});

        // Searching for an entity with the same name should return the same identifier
        CHECK(blueprint.bimap().atLeft("foo") == foo);

        blueprint.clear();
    }

    // If the blueprint is empty/has been initialized then searching for an entity should always
    // return a null identifier.
    CHECK_FALSE(blueprint.bimap().containsRight("foo"));

    // Create two entities on the blueprint.
    auto bar = blueprint.create("bar");
    auto baz = blueprint.create("baz");
    blueprint.add(baz, IntegerComponent{2});

    // Add a ParentComponent to "baz" with id = "bar", using a deserializer.
    {
        // Pack the identifier of "bar".
        auto barPkg = Package::from(bar);

        // Unpack the identifier of "bar" into a ParentComponent.
        Unpackager unpackager{barPkg};
        Registry::create("parent", unpackager, blueprint, baz);
    }

    SUBCASE("spawn the blueprint")
    {
        // Spawn the blueprint into the world and get the identifiers of the spawned entities.
        auto spawned = cmds.spawn(blueprint);
        auto spawnedBar = spawned.entity("bar");
        auto spawnedBaz = spawned.entity("baz");
        cmdBuffer.commit();

        // Package the entities to make sure they were properly spawned.
        auto barPkg = world.pack(spawnedBar);
        auto bazPkg = world.pack(spawnedBaz);

        // "bar" has no components.
        CHECK(barPkg.type() == Package::Type::Object);
        CHECK(barPkg.fields().size() == 0);

        // "baz" has a ParentComponent with parent = "bar" and an IntegerComponent with integer = 2.
        CHECK(bazPkg.type() == Package::Type::Object);
        CHECK(bazPkg.fields().size() == 2);
        CHECK(bazPkg.field("parent").get<Entity>() == spawnedBar);
        CHECK(bazPkg.field("integer").get<int>() == 2);
    }

    SUBCASE("merge one blueprint into another blueprint and then spawn it")
    {
        // Create another blueprint with one entity.
        Blueprint merged{};
        auto foo = merged.create("foo");
        merged.add(foo, IntegerComponent{1});

        // Merge the original blueprint into the new one.
        merged.merge("sub", blueprint);

        // Then the new blueprint has the correct entities.
        CHECK(merged.bimap().atLeft("foo") == foo);
        CHECK(merged.bimap().containsRight("sub.bar"));
        CHECK(merged.bimap().containsRight("sub.baz"));

        // Spawn the blueprint into the world and get the identifiers of the spawned entities.
        auto spawned = cmds.spawn(merged);
        auto spawnedFoo = spawned.entity("foo");
        auto spawnedBar = spawned.entity("sub.bar");
        auto spawnedBaz = spawned.entity("sub.baz");
        cmdBuffer.commit();

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

        // "baz" has a ParentComponent with parent = "bar" and an IntegerComponent with value = 2.
        CHECK(bazPkg.type() == Package::Type::Object);
        CHECK(bazPkg.fields().size() == 2);
        CHECK(bazPkg.field("parent").get<Entity>() == spawnedBar);
        CHECK(bazPkg.field("integer").get<int>() == 2);
    }

    SUBCASE("check if arrays of entities are converted correctly when spawned")
    {
        Blueprint blueprint{};
        auto entity0 = blueprint.create("0");
        auto entity1 = blueprint.create("1");
        blueprint.add(entity0, EntityArrayComponent{{entity0, entity1}});

        // Spawn the blueprint into the world and get the identifiers of the spawned entities.
        auto spawned = cmds.spawn(blueprint);
        auto spawned0 = spawned.entity("0");
        auto spawned1 = spawned.entity("1");
        auto& array = spawned.get<EntityArrayComponent>("0");

        CHECK(array.vec.size() == 2);
        CHECK(array.vec[0] == spawned0);
        CHECK(array.vec[1] == spawned1);
    }

    SUBCASE("check if dictionaries of entities are converted correctly when spawned")
    {
        Blueprint blueprint{};
        auto entity0 = blueprint.create("0");
        auto entity1 = blueprint.create("1");
        blueprint.add(entity0, EntityDictionaryComponent{{{'0', entity0}, {'1', entity1}}});

        // Spawn the blueprint into the world and get the identifiers of the spawned entities.
        auto spawned = cmds.spawn(blueprint);
        auto spawned0 = spawned.entity("0");
        auto spawned1 = spawned.entity("1");
        auto& dict = spawned.get<EntityDictionaryComponent>("0");

        CHECK(dict.map.size() == 2);
        REQUIRE(dict.map.contains('0'));
        REQUIRE(dict.map.contains('1'));
        CHECK(dict.map.at('0') == spawned0);
        CHECK(dict.map.at('1') == spawned1);
    }
}
