#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

/// [Enum declaration]
#include <cubos/core/reflection/reflect.hpp>

enum class Color
{
    Red,
    Green,
    Blue
};

/// [Enum declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;

template <>
CUBOS_REFLECT_EXTERNAL_IMPL(Color)
{
    return Type::create("Color").with(
        EnumTrait{}.withVariant<Color::Red>("Red").withVariant<Color::Green>("Green").withVariant<Color::Blue>("Blue"));
}
/// [Reflection definition]

/// [Using the type]
int main()
{
    using cubos::core::reflection::reflect;

    const auto& colorType = reflect<Color>();
    CUBOS_ASSERT(colorType.has<EnumTrait>());

    const auto& variants = colorType.get<EnumTrait>();
    CUBOS_ASSERT(variants.contains("Red"));
    CUBOS_ASSERT(variants.contains("Green"));
    CUBOS_ASSERT(variants.contains("Blue"));
    CUBOS_ASSERT(!variants.contains("White"));

    Color c = Color::Red;
    CUBOS_ASSERT(variants.at("Red").test(&c));

    c = Color::Green;
    CUBOS_ASSERT(variants.at("Green").test(&c));

    c = Color::Blue;
    CUBOS_ASSERT(variants.at("Blue").test(&c));
    variants.at("Red").set(&c);
    CUBOS_ASSERT(variants.at("Red").test(&c));
    CUBOS_ASSERT(variants.at("Red").name() == "Red");
    CUBOS_ASSERT(variants.size() == 3);

    for (const auto& v : variants)
    {
        CUBOS_INFO("Variant {}", v.name());
    }

    CUBOS_INFO("{}", EnumTrait::toString(Color::Red)); // should print Red
    Color buf;
    CUBOS_ASSERT(EnumTrait::fromString(buf, "Green"));
    CUBOS_INFO("{}", EnumTrait::toString(buf)); // should print Green

    return 0;
}
/// [Using the type]
