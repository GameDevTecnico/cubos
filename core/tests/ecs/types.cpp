#include <doctest/doctest.h>

#include <cubos/core/ecs/types.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include "../utils.hpp"

using cubos::core::ecs::Types;
using cubos::core::reflection::reflect;

TEST_CASE("ecs::Types")
{
    Types types{};
    REQUIRE_FALSE(types.contains("int"));
    types.addComponent(reflect<int>());
    REQUIRE(types.contains("int"));
    REQUIRE(types.id("int").inner == 0);
    REQUIRE(types.id(reflect<int>()).inner == 0);
    REQUIRE(types.type({.inner = 0}).is<int>());
    REQUIRE(types.isComponent({.inner = 0}));
}
