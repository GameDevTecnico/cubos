#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/inherits.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::InheritsTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

struct GrandParent
{
    CUBOS_REFLECT;
};

struct Parent
{
    CUBOS_REFLECT;
};

struct Son
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(GrandParent)
{
    return Type::create("GrandParent");
}

CUBOS_REFLECT_IMPL(Parent)
{
    return Type::create("Parent").with(InheritsTrait::from<GrandParent>());
}

CUBOS_REFLECT_IMPL(Son)
{
    return Type::create("Son").with(InheritsTrait::from<Parent>());
}

TEST_CASE("reflection::InheritsTrait")
{
    const auto& type = reflect<Son>();
    REQUIRE(type.has<InheritsTrait>());
    CHECK(type.get<InheritsTrait>().inherits<Parent>());
    CHECK(type.get<InheritsTrait>().inherits<GrandParent>());
}