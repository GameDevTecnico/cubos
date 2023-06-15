#include "fruit.hpp"

// Since fruit contains an integer and a std::string, we must include the headers which define
// reflection for primitive types and strings.
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/object.hpp>

using cubos::core::reflection::ObjectType;

CUBOS_REFLECT_IMPL(Fruit)
{
    return ObjectType::build("Fruit")
        .field("name", &Fruit::name)
        .field("weight", &Fruit::weight)
        .defaultConstructible<Fruit>()
        .get();
}
