#include <cubos/core/tel/logging.hpp>

/// [MyEntity declaration]
#include <cubos/core/reflection/reflect.hpp>

struct MyEntity
{
    CUBOS_REFLECT;
    uint32_t idx;
    uint32_t generation;
};
/// [MyEntity declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type.hpp>

// Since we're exposing fields of primitive types (uint32_t), its important to
// include the header which defines their reflection.
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(MyEntity)
{
    return Type::create("MyEntity")
        .with(NullableTrait{[](const void* instance) {
                                const auto* ent = static_cast<const MyEntity*>(instance);
                                return ent->idx == UINT32_MAX && ent->generation == UINT32_MAX;
                            },
                            [](void* instance) {
                                auto* ent = static_cast<MyEntity*>(instance);
                                ent->idx = UINT32_MAX;
                                ent->generation = UINT32_MAX;
                            }});
}
/// [Reflection definition]

/// [Example usage]
int main()
{
    using cubos::core::reflection::reflect;

    // Retrieve the reflection information for MyEntity
    const auto& entityType = reflect<MyEntity>();

    // Check if MyEntity has the NullableTrait
    CUBOS_ASSERT(entityType.has<NullableTrait>());

    // Retrieve the NullableTrait for MyEntity
    const auto& nullableTrait = entityType.get<NullableTrait>();

    // Create an instance of MyEntity
    MyEntity ent{1, 1};

    // Check if the instance is not null
    CUBOS_ASSERT(!nullableTrait.isNull(&ent));

    // Set the instance to the null state
    nullableTrait.setToNull(&ent);

    // Check if the instance is now null
    CUBOS_ASSERT(nullableTrait.isNull(&ent));
}
/// [Example usage]
