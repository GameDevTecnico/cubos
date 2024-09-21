/// @file
/// @brief Reflection declaration for `std::variant`.
/// @ingroup core-reflection

#pragma once

#include <variant>

#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/traits/union.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename... Types), (std::variant<Types...>))
{
    auto unionTrait = UnionTrait();

    // Iterate over the types in the variant and reflect them.
    (unionTrait.addVariant(
         reflect<Types>().name(), reflect<Types>(),
         [](const void* instance) {
             return std::holds_alternative<Types>(*static_cast<const std::variant<Types...>*>(instance));
         },
         [](void* instance) { *static_cast<std::variant<Types...>*>(instance) = Types{}; },
         [](const void* instance) -> uintptr_t {
             return reinterpret_cast<uintptr_t>(
                 &std::get<Types>(*static_cast<const std::variant<Types...>*>(instance)));
         }),
     ...);

    auto inner = (... + (reflect<Types>().name() + ", "));
    return Type::create("std::variant<" + inner.substr(0, inner.size() - 2) + ">")
        .with(std::move(unionTrait))
        .with(autoConstructibleTrait<std::variant<Types...>>());
}
