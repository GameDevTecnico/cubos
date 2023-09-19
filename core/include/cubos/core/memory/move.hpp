/// @file
/// @brief Function @ref cubos::core::memory::move.
/// @ingroup core-memory

#pragma once

namespace cubos::core::memory
{
    /// @brief Provides a type which is the same as the given type, but without any references.
    /// @note This is a replacement for `std::remove_reference`, which allows us to avoid including
    /// the entire `<type_traits>` header.
    /// @tparam T
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

    /// @brief Returns an R-value reference to the given value
    /// @note This is a replacement for `std::move`, which allows us to avoid including the entire
    /// `<utility>` header.
    /// @tparam T Value type.
    /// @param value Value to move.
    /// @return Moved value.
    /// @ingroup core-memory
    template <typename T>
    typename RemoveReference<T>::Type&& move(T&& value)
    {
        return static_cast<typename RemoveReference<T>::Type&&>(value);
    }
} // namespace cubos::core::memory
