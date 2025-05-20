#include <cubos/core/tel/logging.hpp>

/// [Scale declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Scale
{
    CUBOS_REFLECT;
    float value = 1.0F;

    Scale() = default;
    Scale(float value)
        : value(value)
    {
    }
};
/// [Scale declaration]

/// [Scale definition]
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Scale)
{
    return Type::create("Scale").with(
        ConstructibleTrait::typed<Scale>().withDefaultConstructor().withCustomConstructor<float>({"value"}).build());
}
/// [Scale definition]

int main()
{
    using cubos::core::reflection::reflect;

    /// [Accessing the trait]
    const auto& scaleType = reflect<Scale>();
    CUBOS_ASSERT(scaleType.has<ConstructibleTrait>());
    const auto& constructible = scaleType.get<ConstructibleTrait>();
    /// [Accessing the trait]

    /// [Creating a default instance]
    // Allocate memory for the instance and default-construct it.
    void* instance = operator new(constructible.size());
    constructible.defaultConstruct(instance);
    CUBOS_ASSERT(static_cast<Scale*>(instance)->value == 1.0F);
    /// [Creating a default instance]

    /// [Destroying the instance]
    // Destroy the instance and deallocate its memory.
    constructible.destruct(instance);
    /// [Destroying the instance]

    /// [Creating a custom instance]
    CUBOS_ASSERT(constructible.customConstructorCount() == 1);
    CUBOS_ASSERT(constructible.customConstructor(0).argCount() == 1);
    CUBOS_ASSERT(constructible.customConstructor(0).argName(0) == "value");
    CUBOS_ASSERT(constructible.customConstructor(0).argType(0).is<float>());

    float value = 2.0F;
    void* args[] = {&value};
    constructible.customConstruct(0, instance, args);
    CUBOS_ASSERT(static_cast<Scale*>(instance)->value == 2.0F);
    /// [Creating a custom instance]

    constructible.destruct(instance);
    operator delete(instance);
}
