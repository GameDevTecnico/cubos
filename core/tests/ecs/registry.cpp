#include <doctest/doctest.h>

#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include "utils.hpp"

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Registry;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;

TEST_CASE("ecs::Registry")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Commands cmds{cmdBuffer};

    // Initially type int isn't registered.
    CHECK(Registry::type("int") == nullptr);

    // After registering, it can now be found.
    Registry::add<int>();
    REQUIRE(Registry::type("int") != nullptr);
    CHECK(Registry::type("int") == &reflect<int>());
}
