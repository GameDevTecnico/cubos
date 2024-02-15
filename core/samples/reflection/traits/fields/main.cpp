#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

/// [Person declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Person
{
    CUBOS_REFLECT;
    int32_t age;
    float weight;
    bool dead;
};
/// [Person declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

// Since we're exposing fields of primitive types (int32_t, float and bool), its important to
// include the header which defines their reflection.
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Person)
{
    return Type::create("Person").with(FieldsTrait()
                                           .withField("age", &Person::age)
                                           .withField("weight", &Person::weight)
                                           .withField("dead", &Person::dead));
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

    CUBOS_INFO("--- Accessing the fields of a type ---");
    /// [Iterating over fields]
    for (const auto& field : fields)
    {
        CUBOS_INFO("Field {} of type {}", field.name(), field.type().name());
    }
    /// [Iterating over fields]

    /// [Iterating over fields output]
    // Field 'age' of type 'int32_t'
    // Field 'weight' of type 'float'
    // Field 'dead' of type 'bool'
    /// [Iterating over fields output]

    CUBOS_INFO("--- Accessing the fields of an instance ---");
    /// [Iterating over fields with data]
    Person person{.age = 21, .weight = 68.4F, .dead = false};
    auto view = fields.view(&person);

    for (auto [field, value] : view)
    {
        if (field->type().is<int32_t>())
        {
            CUBOS_INFO("Field {}: {}", field->name(), *static_cast<int32_t*>(value));
        }
        else if (field->type().is<float>())
        {
            CUBOS_INFO("Field {}: {}", field->name(), *static_cast<float*>(value));
        }
        else
        {
            CUBOS_INFO("Field {}: unsupported type {}", field->name(), field->type().name());
        }
    }
    /// [Iterating over fields with data]

    /// [Iterating over fields with data output]
    // Field 'age': 21
    // Field 'weight': 68.4
    // Field 'dead': unsupported type 'bool'
    /// [Iterating over fields with data output]

    CUBOS_INFO("--- Accessing a specific field of an instance ---");
    /// [Accessing fields by name]
    *static_cast<float*>(view.get("weight")) += 20.0F;
    CUBOS_INFO("New weight: {}", person.weight); // 88.4
}
/// [Accessing fields by name]

/// [Accessing fields by name output]
// New weight: 88.4
/// [Accessing fields by name output]
