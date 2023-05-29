/// @file
/// @brief Covers the SystemInfo and SystemWrapper classes.

#include <doctest/doctest.h>

#include <cubos/core/ecs/system.hpp>

#include "utils.hpp"

using cubos::core::ecs::SystemInfo;
using cubos::core::ecs::SystemWrapper;

TEST_CASE("ecs::SystemInfo")
{
    SystemInfo infoA{};
    SystemInfo infoB{};

    SUBCASE("A is valid")
    {
        SUBCASE("A does nothing")
        {
        }

        SUBCASE("A reads two components")
        {
            infoA.componentsRead.insert(typeid(int));
            infoA.componentsRead.insert(typeid(double));
        }

        SUBCASE("A writes two components and reads another")
        {
            infoA.componentsWritten.insert(typeid(int));
            infoA.componentsWritten.insert(typeid(double));
            infoA.componentsRead.insert(typeid(float));
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
        SUBCASE("A reads and writes to the same component")
        {
            infoA.componentsRead.insert(typeid(int));
            infoA.componentsWritten.insert(typeid(int));
        }

        SUBCASE("A reads and writes to the same resource")
        {
            infoA.resourcesRead.insert(typeid(int));
            infoA.resourcesWritten.insert(typeid(int));
        }

        SUBCASE("A uses the world directly and reads a component")
        {
            infoA.usesWorld = true;
            infoA.componentsRead.insert(typeid(int));
        }

        SUBCASE("A uses the world directly and writes a component")
        {
            infoA.usesWorld = true;
            infoA.componentsWritten.insert(typeid(int));
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

        SUBCASE("both read the same component")
        {
            infoA.componentsRead.insert(typeid(int));
            infoB.componentsRead.insert(typeid(int));
        }

        SUBCASE("both write to different components")
        {
            infoA.componentsWritten.insert(typeid(double));
            infoB.componentsWritten.insert(typeid(float));
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
        SUBCASE("both write to the same component")
        {
            infoA.componentsWritten.insert(typeid(int));
            infoB.componentsWritten.insert(typeid(int));
        }

        SUBCASE("one reads and the other writes to the same component")
        {
            infoA.componentsRead.insert(typeid(int));
            infoB.componentsWritten.insert(typeid(int));
        }

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
