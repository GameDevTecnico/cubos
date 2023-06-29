/// @file
/// @brief Declares `std::vector<T>` as reflectable.

#pragma once

#include <vector>

#include <cubos/core/reflection/array.hpp>

/// @brief Define reflection for all `std::vector<T>` types where `T` is reflectable.
CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T), (std::vector<T>))
{
    return ArrayType::build("std::vector<" + reflect<T>().name() + ">", reflect<T>())
        .length([](const void* array) { return reinterpret_cast<const std::vector<T>*>(array)->size(); })
        .element([](const void* array, std::size_t index) {
            return reinterpret_cast<uintptr_t>(&(*reinterpret_cast<const std::vector<T>*>(array))[index]);
        })
        .resize([](void* array, std::size_t size) { reinterpret_cast<std::vector<T>*>(array)->resize(size); })
        .template typed<std::vector<T>>()
        .get();
}
