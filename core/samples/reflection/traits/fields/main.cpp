#include <cubos/core/log.hpp>

/// [Person declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Person
{
    CUBOS_REFLECT;
    int age;
    float weight;
};
/// [Person declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

// Since we're exposing fields of primitive types (int and float), its important to include the
// header which defines their reflection.
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Person)
{
    return Type::create("Person").with(
        FieldsTrait().withField("age", &Person::age).withField("weight", &Person::weight));
}
/// [Reflection definition]

/// [Accessing the trait]
int main()
{
    using cubos::core::reflection::reflect;

    const auto& personType = reflect<Person>();
    CUBOS_ASSERT(personType.has<FieldsTrait>());
    const auto& fields = personType.get<FieldsTrait>();
    /// [Accessing the trait]

    /// [Iterating over fields]
    for (const auto& field : fields)
    {
        CUBOS_INFO("Field '{}' of type '{}'", field.name(), field.type().name());
    }
    /// [Iterating over fields]

    /// [Accessing fields by name]
    Person person{.age = 21, .weight = 68.4F};
    *static_cast<float*>(fields.view(&person).get(*fields.field("weight"))) += 20.0F;
    CUBOS_INFO("New weight: {}", person.weight); // 88.4
}
/// [Accessing fields by name]
