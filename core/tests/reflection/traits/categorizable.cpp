
#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/categorizable.hpp>
#include <cubos/core/reflection/type.hpp>

#include "cubos/core/reflection/reflect.hpp"

using cubos::core::reflection::CategorizableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

namespace
{
    struct SimpleType
    {
        CUBOS_REFLECT;
    };

    struct PhysicsType
    {
        CUBOS_REFLECT;
        float mass;
    };

    struct AnotherPhysicsType
    {
        CUBOS_REFLECT;
        float force;
    };

} // namespace

CUBOS_REFLECT_IMPL(SimpleType)
{
    return Type::create("SimpleType");
}

CUBOS_REFLECT_IMPL(PhysicsType)
{
    return Type::create("PhysicsType").with(CategorizableTrait{"Physics", 0});
}

CUBOS_REFLECT_IMPL(AnotherPhysicsType)
{
    return Type::create("AnotherPhysicsType").with(CategorizableTrait{"Physics", 1});
}

TEST_CASE("reflection::CategorizableTrait")
{
    SUBCASE("No Category")
    {
        const auto& simpleType = reflect<SimpleType>();
        REQUIRE_FALSE(simpleType.has<CategorizableTrait>());
    }

    SUBCASE("Two physics types")
    {
        const auto& physicsType = reflect<PhysicsType>();
        REQUIRE(physicsType.has<CategorizableTrait>());

        auto trait = physicsType.get<CategorizableTrait>();
        REQUIRE(trait.category() == "Physics");
        REQUIRE(trait.priority() == 0);

        const auto& anotherPhysicsType = reflect<AnotherPhysicsType>();
        REQUIRE(anotherPhysicsType.has<CategorizableTrait>());

        trait = anotherPhysicsType.get<CategorizableTrait>();
        REQUIRE(trait.category() == "Physics");
        REQUIRE(trait.priority() == 1);
    }
}
