#include <doctest/doctest.h>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/component/vec_storage.hpp>
#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "utils.hpp"

using cubos::core::data::old::Package;
using cubos::core::data::old::Unpackager;
using cubos::core::ecs::Blueprint;
using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Registry;
using cubos::core::ecs::VecStorage;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;

TEST_CASE("ecs::Registry")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Commands cmds{cmdBuffer};
    Blueprint blueprint{};
    auto entity = blueprint.create("entity");

    // Initially type int isn't registered.
    CHECK(Registry::type("int") == nullptr);

    // After registering, it can now be found.
    Registry::add<int, VecStorage<int>>();
    REQUIRE(Registry::type("int") != nullptr);
    CHECK(Registry::type("int") == &reflect<int>());

    // Create a storage for it and check if its of the correct type.
    auto storage = Registry::createStorage(reflect<int>());
    CHECK(dynamic_cast<VecStorage<int>*>(storage.get()) != nullptr);

    // Instantiate the component into a blueprint, from a package.
    auto pkg = Package::from<int>(42);
    Unpackager unpackager{pkg};
    REQUIRE(Registry::create("int", unpackager, blueprint, entity));

    // Spawn the blueprint into the world.
    world.registerComponent<int>();
    entity = cmds.spawn(blueprint).entity("entity");
    cmdBuffer.commit();

    // Package the entity and check if the component was correctly instantiated.
    pkg = world.pack(entity);
    REQUIRE(pkg.fields().size() == 1);
    CHECK(pkg.field("int").get<int>() == 42);
}
