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
using cubos::core::ecs::SystemInfo;
using cubos::core::ecs::SystemWrapper;
using cubos::core::ecs::World;

TEST_CASE("ecs::SystemInfo")
{
    SystemInfo infoA{};
    SystemInfo infoB{};

    SUBCASE("A is valid")
    {
        SUBCASE("A does nothing")
        {
        }

        SUBCASE("A reads two resources")
        {
            infoA.resourcesRead.insert(typeid(int));
            infoA.resourcesRead.insert(typeid(double));
        }

        SUBCASE("A writes two resources and reads another")
        {
            infoA.resourcesWritten.insert(typeid(int));
            infoA.resourcesWritten.insert(typeid(double));
            infoA.resourcesRead.insert(typeid(float));
        }

        SUBCASE("A uses commands")
        {
            infoA.usesCommands = true;
        }

        SUBCASE("A uses the world directly")
        {
            infoA.usesWorld = true;
        }

        CHECK(infoA.valid());
    }

    SUBCASE("A is invalid")
    {
        SUBCASE("A reads and writes to the same resource")
        {
            infoA.resourcesRead.insert(typeid(int));
            infoA.resourcesWritten.insert(typeid(int));
        }

        SUBCASE("A uses the world directly and reads a resource")
        {
            infoA.usesWorld = true;
            infoA.resourcesRead.insert(typeid(int));
        }

        SUBCASE("A uses the world directly and writes a resource")
        {
            infoA.usesWorld = true;
            infoA.resourcesWritten.insert(typeid(int));
        }

        SUBCASE("A uses the world directly and uses commands")
        {
            infoA.usesWorld = true;
            infoA.usesCommands = true;
        }

        CHECK_FALSE(infoA.valid());
    }

    SUBCASE("A and B are compatible")
    {
        SUBCASE("both do nothing")
        {
        }

        SUBCASE("both read the same resource")
        {
            infoA.resourcesRead.insert(typeid(int));
            infoB.resourcesRead.insert(typeid(int));
        }

        SUBCASE("both write to different resources")
        {
            infoA.resourcesWritten.insert(typeid(double));
            infoB.resourcesWritten.insert(typeid(float));
        }

        SUBCASE("both use commands")
        {
            infoA.usesCommands = true;
            infoB.usesCommands = true;
        }

        CHECK(infoA.compatible(infoB));
        CHECK(infoB.compatible(infoA));
    }

    SUBCASE("A and B are incompatible")
    {
        SUBCASE("both write to the same resource")
        {
            infoA.resourcesWritten.insert(typeid(int));
            infoB.resourcesWritten.insert(typeid(int));
        }

        SUBCASE("one reads and the other writes to the same resource")
        {
            infoA.resourcesRead.insert(typeid(int));
            infoB.resourcesWritten.insert(typeid(int));
        }

        SUBCASE("one accesses the world directly")
        {
            infoA.usesWorld = true;
        }

        CHECK_FALSE(infoA.compatible(infoB));
        CHECK_FALSE(infoB.compatible(infoA));
    }
}

// Some system functions which do nothing, used to test if the wrapper extracts the correct system
// information.

static void accessNothing()
{
}

static void accessResources(const int& /*unused*/, double& /*unused*/, const float& /*unused*/)
{
}

static void accessComponents(Query<const int&, double&> /*unused*/, Query<const int&, const float&> /*unused*/)
{
}

static void accessCommands(Commands /*unused*/)
{
}

static void accessWriteWorld(World& /*unused*/)
{
}

static void accessReadWorld(const World& /*unused*/)
{
}

/// Utility used to run a system function with the given world.
template <typename F>
static void runSystem(World& world, CommandBuffer& cmdBuf, F f)
{
    SystemWrapper<F> wrapper{f};
    wrapper.prepare(world);
    wrapper.call(world, cmdBuf);
}

TEST_CASE("ecs::SystemWrapper")
{
    SUBCASE("Wrapper extracts the system information correctly")
    {
        SUBCASE("System does not access anything")
        {
            auto info = SystemWrapper(accessNothing).info();
            CHECK(info.resourcesRead.empty());
            CHECK(info.resourcesWritten.empty());
            CHECK_FALSE(info.usesCommands);
            CHECK_FALSE(info.usesWorld);
        }

        SUBCASE("System accesses resources")
        {
            auto info = SystemWrapper(accessResources).info();
            CHECK(info.resourcesRead.size() == 2);
            CHECK(info.resourcesWritten.size() == 1);
            CHECK(info.resourcesRead.contains(typeid(int)));
            CHECK(info.resourcesRead.contains(typeid(float)));
            CHECK(info.resourcesWritten.contains(typeid(double)));
            CHECK_FALSE(info.usesCommands);
            CHECK_FALSE(info.usesWorld);
        }

        SUBCASE("System accesses components")
        {
            auto info = SystemWrapper(accessComponents).info();
            CHECK(info.resourcesRead.empty());
            CHECK(info.resourcesWritten.empty());
            CHECK_FALSE(info.usesCommands);
            CHECK_FALSE(info.usesWorld);
        }

        SUBCASE("System accesses commands")
        {
            auto info = SystemWrapper(accessCommands).info();
            CHECK(info.resourcesRead.empty());
            CHECK(info.resourcesWritten.empty());
            CHECK(info.usesCommands);
            CHECK_FALSE(info.usesWorld);
        }

        SUBCASE("System accesses the world directly")
        {
            SystemInfo info{};

            SUBCASE("System reads the world")
            {
                info = SystemWrapper(accessReadWorld).info();
            }

            SUBCASE("System writes to the world")
            {
                info = SystemWrapper(accessWriteWorld).info();
            }

            CHECK(info.resourcesRead.empty());
            CHECK(info.resourcesWritten.empty());
            CHECK_FALSE(info.usesCommands);
            CHECK(info.usesWorld);
        }
    }

    SUBCASE("Wrapper calls the system correctly")
    {
        World world{};
        CommandBuffer cmdBuf{world};

        SUBCASE("Systems read and write to a resource")
        {
            world.registerResource<int>(0);
            runSystem(world, cmdBuf, [](const int& res) { CHECK(res == 0); });
            runSystem(world, cmdBuf, [](int& res) { res = 1; });
            runSystem(world, cmdBuf, [](const int& res) { CHECK(res == 1); });
        }

        SUBCASE("Systems spawn an entity and read/write from/to its component")
        {
            setupWorld(world);

            // Spawn entity with no components.
            Entity ent;
            runSystem(world, cmdBuf, [&](Commands cmd) { ent = cmd.create().entity(); });
            cmdBuf.commit();

            // Check that the entity has no components.
            runSystem(world, cmdBuf, [&](Query<IntegerComponent&> q) { CHECK_FALSE(q.at(ent).contains()); });

            // Add component.
            runSystem(world, cmdBuf, [&](Commands cmd) { cmd.add(ent, IntegerComponent{0}); });

            // Commands have not been committed yet, so the component should not be readable.
            runSystem(world, cmdBuf, [&](Query<IntegerComponent&> q) { CHECK_FALSE(q.at(ent).contains()); });

            cmdBuf.commit();

            // Write to it.
            runSystem(world, cmdBuf, [&](Query<IntegerComponent&> q) {
                REQUIRE(q.at(ent).contains());
                auto [comp] = *q.at(ent);
                CHECK(comp.value == 0);
                comp.value = 1;
            });

            // Read from it.
            runSystem(world, cmdBuf, [&](Query<const IntegerComponent&> query) {
                auto [comp] = *query.at(ent);
                CHECK(comp.value == 1);
            });
        }
    }
}
