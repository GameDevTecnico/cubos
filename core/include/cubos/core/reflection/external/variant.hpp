/// @file
/// @brief Declares `std::variant<Types...>` as reflectable.

#pragma once

#include <variant>

#include <cubos/core/reflection/variant.hpp>

/// @brief Define reflection for all `std::variant<Types...>` types where all `Types` are
/// reflectable.
CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename... Types), (std::variant<Types...>))
{
    std::string name = "std::variant<";
    ((name += reflect<Types>().name() + ", "), ...);
    name.pop_back();   // Remove last space.
    name.back() = '>'; // Replace last comma with closing angle bracket.

    using Var = std::variant<Types...>;

    auto builder = VariantType::builder(std::move(name));
    builder.index([](const void* instance) -> std::size_t { return static_cast<const Var*>(instance)->index(); });

    // This is very ugly, but there's no way around it.
    // The expression is repeated for each variant in the type (using a fold expression).
    ((builder
          .address([](const void* instance) -> uintptr_t {
              return reinterpret_cast<uintptr_t>(&std::get<Types>(*static_cast<const Var*>(instance)));
          })
          .emplaceDefault([](void* instance) { static_cast<Var*>(instance)->template emplace<Types>(); })
          .template variant<Types>()),
     ...);

    return builder.typed<Var>().get();
}
