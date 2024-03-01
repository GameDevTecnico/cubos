/// @file
/// @brief Endianness utility functions.
/// @ingroup core-memory

#pragma once

#include <cubos/core/api.hpp>

namespace cubos::core::memory
{
    /// @brief Swaps the bytes of a value, changing its endianness.
    /// @tparam T Value type.
    /// @param value Value to swap.
    /// @return Swapped value.
    /// @ingroup core-memory
    template <typename T>
    T swapBytes(T value);

    /// @brief Checks if the current platform is little endian.
    /// @return Whether its little endian.
    /// @ingroup core-memory
    bool isLittleEndian();

    /// @brief Converts a value from little endianness to local endianness.
    /// @tparam T Value type.
    /// @param value Value to convert.
    /// @return Converted value.
    /// @ingroup core-memory
    template <typename T>
    T fromLittleEndian(T value);

    /// @brief Converts a value from local endianness to little endianness.
    /// @tparam T Value type.
    /// @param value Value to convert.
    /// @return Converted value.
    /// @ingroup core-memory
    template <typename T>
    T toLittleEndian(T value);

    /// @brief Converts a value from big endianness to local endianness.
    /// @tparam T Value type.
    /// @param value Value to convert.
    /// @return Converted value.
    /// @ingroup core-memory
    template <typename T>
    T fromBigEndian(T value);

    /// @brief Converts a value from local endianness to big endianness.
    /// @tparam T Value type.
    /// @param value Value to convert.
    /// @return Converted value.
    /// @ingroup core-memory
    template <typename T>
    T toBigEndian(T value);

    // Implementation.

    template <typename T>
    inline T swapBytes(T value)
    {
        union {
            T value;
            unsigned char bytes[sizeof(T)];
        } src, dst;

        src.value = value;
        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            dst.bytes[i] = src.bytes[sizeof(T) - i - 1];
        }
        return dst.value;
    }

    inline bool isLittleEndian()
    {
        int i = 1;
        return *reinterpret_cast<char*>(&i) == 1;
    }

    template <typename T>
    inline T fromLittleEndian(T value)
    {
        return isLittleEndian() ? value : swapBytes(value);
    }

    template <typename T>
    inline T toLittleEndian(T value)
    {
        return isLittleEndian() ? value : swapBytes(value);
    }

    template <typename T>
    inline T fromBigEndian(T value)
    {
        return isLittleEndian() ? swapBytes(value) : value;
    }

    template <typename T>
    inline T toBigEndian(T value)
    {
        return isLittleEndian() ? swapBytes(value) : value;
    }
} // namespace cubos::core::memory
