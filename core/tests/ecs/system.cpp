/// @file
/// @brief Covers the SystemInfo and SystemWrapper classes.

#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "utils.hpp"

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Query;
using cubos::core::ecs::System;
using cubos::core::ecs::World;

/// Utility used to run a system function with the given world.
template <typename T>
static T runSystem(World& world, CommandBuffer& cmdBuf, auto f)
{
    return System<T>::make(world, f, {}).run(cmdBuf);
}

TEST_CASE("ecs::System")
{
    World world{};
    CommandBuffer cmdBuf{world};

    world.registerResource<int>(0);
    world.registerComponent<bool>();
    auto boolId = world.types().id("bool");

    SUBCASE("extracted access patterns are correct")
    {
        SUBCASE("no arguments")
        {
            auto system = System<void>::make(world, []() {});
            CHECK_FALSE(system.access().usesWorld);
            CHECK(system.access().dataTypes.empty());
        }

        SUBCASE("world")
        {
            auto system = System<void>::make(world, [](World&) {});
            CHECK(system.access().usesWorld);
            CHECK(system.access().dataTypes.empty());
        }

        SUBCASE("const world")
        {
            auto system = System<void>::make(world, [](const World&) {});
            CHECK(system.access().usesWorld);
            CHECK(system.access().dataTypes.empty());
        }

        SUBCASE("query")
        {
            auto system = System<void>::make(world, [](Query<bool&>) {});
            CHECK_FALSE(system.access().usesWorld);
            CHECK(system.access().dataTypes.contains(boolId));
            CHECK(system.access().dataTypes.size() == 1);
        }
    }

    SUBCASE("system runs correctly")
    {
        SUBCASE("read and write to a resource")
        {
            runSystem<void>(world, cmdBuf, [](const int& res) { CHECK(res == 0); });
            runSystem<void>(world, cmdBuf, [](int& res) { res = 1; });
            runSystem<void>(world, cmdBuf, [](const int& res) { CHECK(res == 1); });

            int ret = runSystem<int>(world, cmdBuf, [](const int& res) { return res; });
            CHECK(ret == 1);
        }

        SUBCASE("spawn entity and then access it")
        {
            // Spawn entity with no components.
            Entity ent;
            runSystem<void>(world, cmdBuf, [&](Commands cmd) { ent = cmd.create().entity(); });
            cmdBuf.commit();

            // Check that the entity has no components.
            runSystem<void>(world, cmdBuf, [&](Query<bool&> q) { CHECK_FALSE(q.at(ent).contains()); });

            // Add component.
            runSystem<void>(world, cmdBuf, [&](Commands cmd) { cmd.add(ent, false); });

            // Commands have not been committed yet, so the component should not be readable.
            runSystem<void>(world, cmdBuf, [&](Query<bool&> q) { CHECK_FALSE(q.at(ent).contains()); });

            cmdBuf.commit();

            // Write to it.
            runSystem<void>(world, cmdBuf, [&](Query<bool&> q) {
                REQUIRE(q.at(ent).contains());
                auto [comp] = *q.at(ent);
                CHECK_FALSE(comp);
                comp = true;
            });

            // Read from it.
            runSystem<void>(world, cmdBuf, [&](Query<const bool&> query) {
                auto [comp] = *query.at(ent);
                CHECK(comp);
            });
        }
    }
}
