#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/hidden.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::HiddenTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

namespace
{
    struct SimpleType
    {
        CUBOS_REFLECT;
    };

    struct HiddenType
    {
        CUBOS_REFLECT;
        float mass;
    };

} // namespace

CUBOS_REFLECT_IMPL(SimpleType)
{
    return Type::create("SimpleType");
}

CUBOS_REFLECT_IMPL(HiddenType) 
{
    return Type::create("HiddenType").with(HiddenTrait{true});
}

TEST_CASE("reflection::HiddenTrait")
{
    SUBCASE("Not hidden")
    {
        const auto& simpleType = reflect<SimpleType>();
        REQUIRE_FALSE(simpleType.has<SimpleType>());
    }

    SUBCASE("Hidden")
    {
        const auto& hiddenType = reflect<HiddenType>();
        REQUIRE(hiddenType.has<HiddenTrait>());
        const auto& trait = hiddenType.get<HiddenTrait>();
        REQUIRE(trait.hidden());
    }
}
