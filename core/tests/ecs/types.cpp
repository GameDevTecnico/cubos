#include <doctest/doctest.h>

#include <cubos/core/ecs/types.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include "utils.hpp"

using cubos::core::ecs::Types;
using cubos::core::reflection::reflect;

TEST_CASE("ecs::Types")
{
    Types types{};
    REQUIRE_FALSE(types.contains("int"));
    types.addComponent(reflect<int>());
    REQUIRE(types.contains(reflect<int>()));
    REQUIRE(types.contains("int"));
    REQUIRE(types.id("int").inner == 0);
    REQUIRE(types.id(reflect<int>()).inner == 0);
    REQUIRE(types.type({.inner = 0}).is<int>());
    REQUIRE(types.isComponent({.inner = 0}));
    REQUIRE_FALSE(types.isRelation({.inner = 0}));

    types.addRelation(reflect<SymmetricRelation>());
    REQUIRE(types.contains(reflect<SymmetricRelation>()));
    REQUIRE(types.contains("SymmetricRelation"));
    REQUIRE(types.id("SymmetricRelation").inner == 1);
    REQUIRE(types.id(reflect<SymmetricRelation>()).inner == 1);
    REQUIRE(types.type({.inner = 1}).is<SymmetricRelation>());
    REQUIRE_FALSE(types.isComponent({.inner = 1}));
    REQUIRE(types.isRelation({.inner = 1}));
    REQUIRE(types.isSymmetricRelation({.inner = 1}));
    REQUIRE_FALSE(types.isTreeRelation({.inner = 1}));

    types.addRelation(reflect<TreeRelation>());
    REQUIRE(types.contains(reflect<TreeRelation>()));
    REQUIRE(types.contains("TreeRelation"));
    REQUIRE(types.id("TreeRelation").inner == 2);
    REQUIRE(types.id(reflect<TreeRelation>()).inner == 2);
    REQUIRE(types.type({.inner = 2}).is<TreeRelation>());
    REQUIRE_FALSE(types.isComponent({.inner = 2}));
    REQUIRE(types.isRelation({.inner = 2}));
    REQUIRE(types.isTreeRelation({.inner = 2}));
    REQUIRE_FALSE(types.isSymmetricRelation({.inner = 2}));
}
