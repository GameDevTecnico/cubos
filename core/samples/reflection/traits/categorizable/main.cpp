#include "cubos/core/tel/logging.hpp"

/// [Transform Entities declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Position
{
    CUBOS_REFLECT;

    float x, y, z;
};

struct Scale
{
    CUBOS_REFLECT;

    float factor{1.0F};
};
/// [Transform Entities declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/categorizable.hpp>
#include <cubos/core/reflection/type.hpp>

// The type builder can be used the same way with the method `categorize`.
#include <cubos/core/ecs/reflection.hpp>

// Since we're exposing fields of a primitive type (float), it's important to
// include the header which defines their reflection.
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::TypeBuilder;
using cubos::core::reflection::CategorizableTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Position)
{
    return Type::create("Position").with(CategorizableTrait("Transform", 0));
}

CUBOS_REFLECT_IMPL(Scale)
{
    // We can also use the type builder and categorize the components directly.
    return TypeBuilder<Scale>("Scale").categorize("Transform", 1).wrap(&Scale::factor);
}
/// [Reflection definition]

/// [Accessing the trait]
int main()
{
    using cubos::core::reflection::reflect;

    // Retrieve the reflection information for Position.
    const auto& positionType = reflect<Position>();
    CUBOS_ASSERT(positionType.has<CategorizableTrait>());
    auto categorizableTrait = positionType.get<CategorizableTrait>();
    /// [Accessing the trait]

    /// [Checking the category and priority of a type]
    CUBOS_ASSERT(categorizableTrait.category() == "Transform");
    CUBOS_ASSERT(categorizableTrait.priority() == 0);
    /// [Checking the category and priority of a type]

    /// [Scale type]
    const auto& scaleType = reflect<Scale>();
    CUBOS_ASSERT(scaleType.has<CategorizableTrait>());
    categorizableTrait = scaleType.get<CategorizableTrait>();
    CUBOS_ASSERT(categorizableTrait.category() == "Transform");
    CUBOS_ASSERT(categorizableTrait.priority() == 1);
}
/// [Scale type]
