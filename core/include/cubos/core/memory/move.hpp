/// @file
/// @brief Functions @ref cubos::core::memory::move and @ref cubos::core::memory::forward.
/// @ingroup core-memory

#pragma once

namespace cubos::core::memory
{
    /// @brief Provides a type which is the same as the given type, but without any references.
    /// @note This is a replacement for `std::remove_reference`, which allows us to avoid including
    /// the entire `<type_traits>` header.
    /// @tparam T Type.
    template <typename T>
    struct RemoveReference
    {
        /// @brief Type without references.
        using Type = T;
    };

    template <typename T>
    struct RemoveReference<T&>
    {
        using Type = T;
    };

    template <typename T>
    struct RemoveReference<T&&>
    {
        using Type = T;
    };

    /// @brief Used to check if a type is an rvalue reference.
    /// @tparam T Type.
    template <typename T>
    struct IsLValueReference
    {
        static constexpr bool Value = false;
    };

    template <typename T>
    struct IsLValueReference<T&>
    {
        static constexpr bool Value = true;
    };

    /// @brief Returns an R-value reference to the given value
    /// @note This is a replacement for `std::move`, which allows us to avoid including the entire
    /// `<utility>` header.
    /// @tparam T Value type.
    /// @param value Value to move.
    /// @return Moved value.
    /// @ingroup core-memory
    template <typename T>
    typename RemoveReference<T>::Type&& move(T&& value) noexcept
    {
        return static_cast<typename RemoveReference<T>::Type&&>(value);
    }

    /// @brief Used to cast a templated function parameter to the value category the caller used to
    /// pass it, which allows rvalues to be passed as rvalues and lvalues as lvalues.
    /// @note This is a replacement for `std::forward`, which allows us to avoid including the
    /// entire `<utility>` header.
    /// @tparam T Argument type.
    /// @param argument Argument to forward.
    /// @return Forwarded argument.
    /// @ingroup core-memory
    template <typename T>
    T&& forward(typename RemoveReference<T>::Type& argument) noexcept
    {
        return static_cast<T&&>(argument);
    }

    /// @copydoc forward
    template <typename T>
    T&& forward(typename RemoveReference<T>::Type&& argument) noexcept
    {
        static_assert(!IsLValueReference<T>::Value, "Cannot forward an rvalue as an lvalue");
        return static_cast<T&&>(argument);
    }
} // namespace cubos::core::memory
