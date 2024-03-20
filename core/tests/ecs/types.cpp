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
    REQUIRE_FALSE(types.isResource({.inner = 0}));
    REQUIRE(types.isComponent({.inner = 0}));
    REQUIRE_FALSE(types.isRelation({.inner = 0}));

    types.addRelation(reflect<SymmetricRelation>());
    REQUIRE(types.contains(reflect<SymmetricRelation>()));
    REQUIRE(types.contains("SymmetricRelation"));
    REQUIRE(types.id("SymmetricRelation").inner == 1);
    REQUIRE(types.id(reflect<SymmetricRelation>()).inner == 1);
    REQUIRE(types.type({.inner = 1}).is<SymmetricRelation>());
    REQUIRE_FALSE(types.isResource({.inner = 1}));
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
    REQUIRE_FALSE(types.isResource({.inner = 2}));
    REQUIRE_FALSE(types.isComponent({.inner = 2}));
    REQUIRE(types.isRelation({.inner = 2}));
    REQUIRE(types.isTreeRelation({.inner = 2}));
    REQUIRE_FALSE(types.isSymmetricRelation({.inner = 2}));

    REQUIRE_FALSE(types.contains("float"));
    types.addResource(reflect<float>());
    REQUIRE(types.contains(reflect<float>()));
    REQUIRE(types.contains("float"));
    REQUIRE(types.id("float").inner == 3);
    REQUIRE(types.id(reflect<float>()).inner == 3);
    REQUIRE(types.id(reflect<float>()).inner == 3);
    REQUIRE(types.isResource({.inner = 3}));
    REQUIRE_FALSE(types.isComponent({.inner = 3}));
    REQUIRE_FALSE(types.isRelation({.inner = 3}));

    auto components = types.components();
    REQUIRE(components.size() == 1);
    REQUIRE(components.contains<int>());
    types.remove(reflect<int>());
    REQUIRE_FALSE(types.contains(reflect<int>()));
    REQUIRE_FALSE(types.contains("int"));
    components = types.components();
    REQUIRE(components.size() == 0);

    auto relations = types.relations();
    REQUIRE(relations.size() == 2);
    REQUIRE(relations.contains<SymmetricRelation>());
    REQUIRE(relations.contains<TreeRelation>());
    types.remove(reflect<SymmetricRelation>());
    REQUIRE_FALSE(types.contains(reflect<SymmetricRelation>()));
    REQUIRE_FALSE(types.contains("SymmetricRelation"));
    relations = types.relations();
    REQUIRE(relations.size() == 1);
    REQUIRE_FALSE(relations.contains<SymmetricRelation>());
    REQUIRE(relations.contains<TreeRelation>());

    auto resources = types.resources();
    REQUIRE(resources.size() == 1);
    REQUIRE(resources.contains<float>());
    types.remove(reflect<float>());
    REQUIRE_FALSE(types.contains(reflect<float>()));
    REQUIRE_FALSE(types.contains("float"));
    resources = types.resources();
    REQUIRE(resources.size() == 0);

    types.addComponent(reflect<int>());
    REQUIRE(types.contains(reflect<int>()));
    REQUIRE(types.contains("int"));
    REQUIRE(types.id("int").inner == 4);
    REQUIRE(types.id(reflect<int>()).inner == 4);
    REQUIRE(types.type({.inner = 4}).is<int>());
    REQUIRE_FALSE(types.isResource({.inner = 4}));
    REQUIRE(types.isComponent({.inner = 4}));
    REQUIRE_FALSE(types.isRelation({.inner = 4}));
}
