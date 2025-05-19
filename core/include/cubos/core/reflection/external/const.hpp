/// @file
/// @brief Reflection declaration for `const T`.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/traits/constant.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T), (const T))
{
    const auto& inner = reflect<T>();
    return Type::create("const " + inner.name())
        .with(ConstantTrait{inner})
        .with(inner.template get<ConstructibleTrait>());
}
