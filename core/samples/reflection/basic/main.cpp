#include <cubos/core/log.hpp>

/// [Person definition]
#include <cubos/core/reflection/reflect.hpp>

struct Person
{
    CUBOS_REFLECT;
    int age;
    float weight;
};
/// [Person definition]

/// [Person reflection]
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Person)
{
    return Type::create("Person");
}
/// [Person reflection]

/// [Position definition]
struct Position
{
    CUBOS_REFLECT;
    float x, y;
};
/// [Position definition]

/// [Your own trait]
struct ColorTrait
{
    CUBOS_REFLECT;
    float r, g, b;
};
/// [Your own trait]

/// [Adding your own trait]
CUBOS_REFLECT_IMPL(ColorTrait)
{
    return Type::create("ColorTrait");
}

CUBOS_REFLECT_IMPL(Position)
{
    return Type::create("Position").with(ColorTrait{.r = 0.0F, .g = 1.0F, .b = 0.0F});
}
/// [Adding your own trait]

/// [Accessing type name]
int main()
{
    using cubos::core::reflection::reflect;

    const auto& personType = reflect<Person>();
    CUBOS_ASSERT(personType.name() == "Person");
    /// [Accessing type name]

    /// [Checking traits]
    const auto& positionType = reflect<Position>();
    CUBOS_ASSERT(positionType.has<ColorTrait>());
    CUBOS_ASSERT(!personType.has<ColorTrait>());
    /// [Checking traits]

    /// [Accessing traits]
    const auto& colorTrait = positionType.get<ColorTrait>();
    CUBOS_ASSERT(colorTrait.r == 0.0F);
    CUBOS_ASSERT(colorTrait.g == 1.0F);
    CUBOS_ASSERT(colorTrait.b == 0.0F);
    return 0;
}
/// [Accessing traits]
