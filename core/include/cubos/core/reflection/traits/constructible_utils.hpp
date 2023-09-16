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

        if (std::is_default_constructible<T>::value)
        {
            builder = static_cast<ConstructibleTrait::Builder<T>&&>(builder).withDefaultConstructor();
        }

        if (std::is_copy_constructible<T>::value)
        {
            builder = static_cast<ConstructibleTrait::Builder<T>&&>(builder).withCopyConstructor();
        }

        if (std::is_move_constructible<T>::value)
        {
            builder = static_cast<ConstructibleTrait::Builder<T>&&>(builder).withMoveConstructor();
        }

        return static_cast<ConstructibleTrait::Builder<T>&&>(builder).build();
    }
} // namespace cubos::core::reflection
