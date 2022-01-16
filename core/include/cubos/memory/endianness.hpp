#ifndef CUBOS_MEMORY_ENDIANNESS_HPP
#define CUBOS_MEMORY_ENDIANNESS_HPP

namespace cubos::memory
{
    /// Swaps the bytes of a value, changing the endianness.
    /// @tparam T The type of the value.
    /// @param value The value to swap.
    /// @return The swapped value.
    template <typename T> T swapBytes(T value);

    /// Checks if the current platform is little endian.
    /// @return True if the current platform is little endian, false otherwise.
    bool isLittleEndian();

    /// Converts a value from little endianness to local endianness.
    /// @tparam T The type of the value.
    /// @param value The value to convert.
    /// @return The converted value.
    template <typename T> T fromLittleEndian(T value);

    /// Converts a value from local endianness to little endianness.
    /// @tparam T The type of the value.
    /// @param value The value to convert.
    /// @return The converted value.
    template <typename T> T toLittleEndian(T value);

    /// Converts a value from big endianness to local endianness.
    /// @tparam T The type of the value.
    /// @param value The value to convert.
    /// @return The converted value.
    template <typename T> T fromBigEndian(T value);

    /// Converts a value from local endianness to big endianness.
    /// @tparam T The type of the value.
    /// @param value The value to convert.
    /// @return The converted value.
    template <typename T> T toBigEndian(T value);

    // Implementation.

    template <typename T> inline T swapBytes(T value)
    {
        union {
            T value;
            unsigned char bytes[sizeof(T)];
        } src, dst;

        src.value = value;
        for (size_t i = 0; i < sizeof(T); i++)
            dst.bytes[i] = src.bytes[sizeof(T) - i - 1];
        return dst.value;
    }

    inline bool isLittleEndian()
    {
        int i = 1;
        return *reinterpret_cast<char*>(&i) == 1;
    }

    template <typename T> inline T fromLittleEndian(T value)
    {
        return isLittleEndian() ? value : swapBytes(value);
    }

    template <typename T> inline T toLittleEndian(T value)
    {
        return isLittleEndian() ? value : swapBytes(value);
    }

    template <typename T> inline T fromBigEndian(T value)
    {
        return isLittleEndian() ? swapBytes(value) : value;
    }

    template <typename T> inline T toBigEndian(T value)
    {
        return isLittleEndian() ? swapBytes(value) : value;
    }
} // namespace cubos::memory

#endif // CUBOS_MEMORY_ENDIANNESS_HPP
