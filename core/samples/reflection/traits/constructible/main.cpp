#include <cubos/core/tel/logging.hpp>

/// [Scale declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Scale
{
    CUBOS_REFLECT;
    float value = 1.0F;
};
/// [Scale declaration]

/// [Scale definition]
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Scale)
{
    return Type::create("Scale").with(ConstructibleTrait::typed<Scale>().withDefaultConstructor().build());
}
/// [Scale definition]

/// [Accessing the trait]
int main()
{
    using cubos::core::reflection::reflect;

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
    operator delete(instance);
}
/// [Destroying the instance]
