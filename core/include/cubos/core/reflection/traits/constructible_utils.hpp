/// @file
/// @brief Utilities for @ref cubos::core::reflection::ConstructibleTrait.
/// @note Avoid using this header as it includes `<type_traits>`.
/// @ingroup core-reflection

#pragma once

#include <type_traits>

#include <cubos/core/reflection/traits/constructible.hpp>

namespace cubos::core::reflection
{
    /// @brief Returns a ConstructibleTrait with the default, copy and move constructors, set only
    /// if the type has them.
    /// @warning Avoid using this function as its header includes `<type_traits>`.
    /// @tparam T Type.
    /// @return ConstructibleTrait.
    /// @ingroup core-reflection
    template <typename T>
    ConstructibleTrait autoConstructibleTrait()
    {
        auto builder = ConstructibleTrait::typed<T>();

        if constexpr (std::is_default_constructible<T>::value)
        {
            builder = memory::move(builder).withDefaultConstructor();
        }

        if constexpr (std::is_copy_constructible<T>::value)
        {
            builder = memory::move(builder).withCopyConstructor();
        }

        if constexpr (std::is_move_constructible<T>::value)
        {
            builder = memory::move(builder).withMoveConstructor();
        }

        return memory::move(builder).build();
    }
} // namespace cubos::core::reflection
