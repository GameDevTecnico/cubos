/// @file
/// @brief Reflection declaration for `std::vector`.
/// @ingroup core-reflection

#pragma once

#include <vector>

#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T), (std::vector<T>))
{
    // This is a bit scary but isn't doing anything too crazy.
    // We are supplying, first, the element type, and then a getter for the length and another for
    // the address of an element.

    auto arrayTrait = ArrayTrait(
        reflect<T>(),
        [](const void* instance) -> std::size_t { return static_cast<const std::vector<T>*>(instance)->size(); },
        [](const void* instance, std::size_t index) -> uintptr_t {
            return reinterpret_cast<uintptr_t>(static_cast<const std::vector<T>*>(instance)->data() + index);
        });

    // We supply the insert functions depending on the constructibility of the element type.

    if constexpr (std::is_default_constructible<T>::value)
    {
        arrayTrait.setInsertDefault([](void* instance, std::size_t index) {
            auto* vec = static_cast<std::vector<T>*>(instance);
            vec->emplace(vec->begin() + static_cast<std::ptrdiff_t>(index));
        });
    }

    if constexpr (std::is_copy_constructible<T>::value)
    {
        arrayTrait.setInsertCopy([](void* instance, std::size_t index, const void* value) {
            auto* vec = static_cast<std::vector<T>*>(instance);
            vec->emplace(vec->begin() + static_cast<std::ptrdiff_t>(index), *static_cast<const T*>(value));
        });
    }

    // Since T must be moveable for all std::vector<T>, we always supply this function.
    arrayTrait.setInsertMove([](void* instance, std::size_t index, void* value) {
        auto* vec = static_cast<std::vector<T>*>(instance);
        vec->emplace(vec->begin() + static_cast<std::ptrdiff_t>(index), std::move(*static_cast<T*>(value)));
    });

    // We supply the erase function always, as it is always possible to erase an element of a vector.
    arrayTrait.setErase([](void* instance, std::size_t index) {
        auto* vec = static_cast<std::vector<T>*>(instance);
        vec->erase(vec->begin() + static_cast<std::ptrdiff_t>(index));
    });

    return Type::create("std::vector<" + reflect<T>().name() + ">")
        .with(arrayTrait)
        .with(autoConstructibleTrait<std::vector<T>>());
}
