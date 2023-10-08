#include "utils.hpp"

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(DetectDestructor)
{
    return Type::create("DetectDestructor")
        .with(ConstructibleTrait::typed<DetectDestructor>().withDefaultConstructor().withMoveConstructor().build());
}
