#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

namespace
{
    enum class Color
    {
        Red,
        Green,
        Blue
    };
} // namespace

CUBOS_REFLECT_EXTERNAL_DECL(Color);
CUBOS_REFLECT_EXTERNAL_IMPL(Color)
{
    return Type::create("Color").with(
        EnumTrait{}.withVariant<Color::Red>("Red").withVariant<Color::Green>("Green").withVariant<Color::Blue>("Blue"));
}

TEST_CASE("reflection::EnumTrait")
{
    Color c;

    const auto& colorType = reflect<Color>();
    REQUIRE(colorType.has<EnumTrait>());

    const auto& variants = colorType.get<EnumTrait>();
    REQUIRE(variants.contains("Red"));
    REQUIRE(variants.contains("Green"));
    REQUIRE(variants.contains("Blue"));
    REQUIRE(!variants.contains("White"));

    auto it = variants.begin();
    CHECK(it->name() == "Red");
    CHECK(it != variants.end());
    CHECK(++it != variants.end());
    CHECK(++it != variants.end());
    CHECK(++it == variants.end());

    c = Color::Red;
    REQUIRE(variants.at("Red").test(&c));
    REQUIRE(variants.variant(&c).test(&c));

    c = Color::Green;
    REQUIRE(variants.at("Green").test(&c));

    c = Color::Blue;
    REQUIRE(variants.at("Blue").test(&c));
    variants.at("Red").set(&c);
    REQUIRE(variants.at("Red").test(&c));
    REQUIRE(variants.at("Red").name() == "Red");
    REQUIRE(variants.size() == 3);

    REQUIRE("Red" == EnumTrait::toString(Color::Red));

    Color buf;
    REQUIRE(!EnumTrait::fromString(buf, "White"));
    REQUIRE(EnumTrait::fromString(buf, "Green"));
    REQUIRE("Green" == EnumTrait::toString(buf));
}
