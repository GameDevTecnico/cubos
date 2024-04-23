#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

TEST_CASE("reflection::Type")
{
    auto& type = Type::create("Foo");
    CHECK(type.name() == "Foo");
    CHECK(type.shortName() == "Foo");

    auto& namespacedType = Type::create("cubos::core::reflection::Foo");
    CHECK(namespacedType.name() == "cubos::core::reflection::Foo");
    CHECK(namespacedType.shortName() == "Foo");

    SUBCASE("without traits")
    {
        CHECK_FALSE(type.has<bool>());
        CHECK_FALSE(type.has<int>());
    }

    SUBCASE("with one trait")
    {
        type.with<bool>(false);
        REQUIRE(type.has<bool>());
        CHECK_FALSE(type.has<int>());
        CHECK_FALSE(type.get<bool>());
    }

    SUBCASE("with two traits")
    {
        type.with<bool>(true);
        type.with<int>(42);
        REQUIRE(type.has<bool>());
        REQUIRE(type.has<int>());
        CHECK(type.get<bool>());
        CHECK(type.get<int>() == 42);
    }

    SUBCASE("equality")
    {
        auto& anotherType = type;
        REQUIRE(anotherType == type);
    }

    SUBCASE("inequality")
    {
        auto& anotherType = Type::create("Bar");
        REQUIRE(anotherType != type);
        Type::destroy(anotherType);
    }

    Type::destroy(type);
}
