/// @file
/// @brief Class @ref cubos::core::memory::Stream.
/// @ingroup core-memory

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <cubos/core/api.hpp>

namespace cubos::core::memory
{
    /// @brief Stream seek origin.
    /// @ingroup core-memory
    enum class SeekOrigin
    {
        Begin,
        Current,
        End
    };

    /// @brief Interface class for memory streams. Abstracts away sources or destinations of data.
    ///
    /// ## Motivation
    ///
    /// Why do we have our own streams? Well, the standard library streams are hard to use and
    /// extend, and very template heavy. Using our own streams allows us to abstract away more
    /// easily where the data is coming from, or going to, allowing us, for example, to embed
    /// files into the executable and not have to worry about the code that reads them.
    ///
    /// @ingroup core-memory
    class CUBOS_CORE_API Stream
    {
    public:
        virtual ~Stream() = default;

        /// @brief Constructs.
        Stream() = default;

        /// @brief Move constructs.
        Stream(Stream&&) = default;

        /// @name Forbid any kind of copying.
        /// @{
        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;
        /// @}

        static Stream& stdIn;  ///< Stream wrapper for `stdin`.
        static Stream& stdOut; ///< Stream wrapper for `stdout`.
        static Stream& stdErr; ///< Stream wrapper for `stderr`.

        /// @brief Reads data from the stream.
        /// @param data Buffer to read data into.
        /// @param size Size of the buffer.
        /// @note Data may be read partially, @ref readExact is recommended.
        /// @return Number of bytes read.
        virtual std::size_t read(void* data, std::size_t size) = 0;

        /// @brief Writes data to the stream.
        /// @param data Buffer to write data from.
        /// @param size Size of the buffer.
        /// @note Data may be written partially, @ref writeExact is recommended.
        /// @return Number of bytes written.
        virtual std::size_t write(const void* data, std::size_t size) = 0;

        /// @brief Reads data from the stream with the exact specified size.
        /// @param data Buffer to read data into.
        /// @param size Size of the buffer.
        /// @note This function will only return after buffer is completed or an error occurs.
        /// @return Whether reading was sucessful.
        bool readExact(void* data, std::size_t size);

        /// @brief Writes data to the stream with the exact specified size.
        /// @param data Buffer to write data from.
        /// @param size Size of the buffer.
        /// @note This function will only return after whole buffer is written or an error occurs.
        /// @return Whether writing was sucessful.
        bool writeExact(const void* data, std::size_t size);

        /// @brief Gets the current position in the stream.
        /// @return Current position in the stream, or SIZE_MAX if the position is unknown.
        virtual std::size_t tell() const = 0;

        /// @brief Seeks to a position in the stream.
        /// @param offset Offset to seek to.
        /// @param origin Origin of the offset.
        virtual void seek(ptrdiff_t offset, SeekOrigin origin) = 0;

        /// @brief Checks if the stream still has content to read.
        /// @return Whether the stream has reached the end.
        virtual bool eof() const = 0;

        /// @brief Peeks one byte from the stream.
        /// @return Peeked byte.
        virtual char peek() = 0;

        /// @brief Gets one byte from the stream.
        /// @return Read byte.
        char get();

        /// @brief Puts one byte into the stream.
        /// @param c Byte to put.
        void put(char c);

        /// @brief Prints a signed integer to the stream.
        /// @tparam T Type of the integer.
        /// @param value Value to print.
        /// @param base Base to use.
        template <typename T>
        requires(::std::is_integral_v<T>&& ::std::is_signed_v<T>) void print(T value, std::size_t base = 10);

        /// @brief Prints a signed integer to the stream.
        /// @tparam T Integer type.
        /// @param value Value to print.
        /// @param base Base to use.
        template <typename T>
        requires(::std::is_integral_v<T> && !::std::is_signed_v<T>) inline void print(T value, std::size_t base = 10);

        /// @brief Prints a 64 bit signed integer to the stream.
        /// @param value Value to print.
        /// @param base Base to use.
        void print(int64_t value, std::size_t base = 10);

        /// @brief Prints a 64 bit unsigned integer to the stream.
        /// @param value Value to print.
        /// @param base Base to use.
        void print(uint64_t value, std::size_t base = 10);

        /// @brief Prints a float to the stream.
        /// @param value Value to print.
        /// @param decimalPlaces Number of decimal places to print.
        void print(float value, std::size_t decimalPlaces = 4);

        /// @brief Prints a double to the stream.
        /// @param value Value to print.
        /// @param decimalPlaces Number of decimal places to print.
        void print(double value, std::size_t decimalPlaces = 4);

        /// @brief Prints a string to the stream.
        /// @param str String to print.
        void print(const char* str);

        /// @brief Prints a string to the stream.
        /// @param str String to print.
        /// @param size Size of the string.
        void print(const char* str, std::size_t size);

        /// @brief Prints a string to the stream.
        /// @param str Value to print.
        void print(const ::std::string& str);

        /// @brief Prints a formatted string the stream.
        ///
        /// ## Usage
        ///
        /// @code{.cpp}
        ///     stream.printf("Hello, {}!\n", "world");
        ///     stream.printf("{} + {} = {}\n", 1, 2, 3);
        ///     stream.printf("\\{} {}\n", 1, 2); // This will print "{} 2"
        /// @endcode
        ///
        /// @tparam T Type of the first argument.
        /// @tparam TArgs Types of the remaining arguments.
        /// @param fmt Format string.
        /// @param arg First argument to print.
        /// @param args Remaining arguments to print.
        template <typename T, typename... TArgs>
        void printf(const char* fmt, T arg, TArgs... args);

        /// @brief Prints a string to the stream.
        /// @note This overload is used to terminate the recursion in @ref printf().
        /// @param fmt Format string.
        void printf(const char* fmt);

        /// @brief Parses a 8 bit signed integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(int8_t& value, std::size_t base = 10);

        /// @brief Parses a 16 bit signed integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(int16_t& value, std::size_t base = 10);

        /// @brief Parses a 32 bit signed integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(int32_t& value, std::size_t base = 10);

        /// @brief Parses a 64 bit signed integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(int64_t& value, std::size_t base = 10);

        /// @brief Parses a 8 bit unsigned integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(uint8_t& value, std::size_t base = 10);

        /// @brief Parses a 16 bit unsigned integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(uint16_t& value, std::size_t base = 10);

        /// @brief Parses a 32 bit unsigned integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(uint32_t& value, std::size_t base = 10);

        /// @brief Parses a 64 bit unsigned integer from the stream.
        /// @param[out] value Parsed value.
        /// @param base Base to use.
        void parse(uint64_t& value, std::size_t base = 10);

        /// @brief Parses a float from the stream.
        /// @param[out] value Parsed value.
        void parse(float& value);

        /// @brief Parses a double from the stream.
        /// @param[out] value Parsed value.
        void parse(double& value);

        /// @brief Reads a string from the stream until the @p terminator (or `\0`) is found.
        /// @param[out] str Read string.
        /// @param terminator Optional terminator to use.
        void readUntil(std::string& str, const char* terminator);

        /// @brief Reads a string from the stream until the @p terminator (or `\0`) is found.
        /// @param buffer Buffer to read into.
        /// @param size Size of the buffer.
        /// @param terminator Optional terminator to use.
        /// @return Number of bytes read.
        std::size_t readUntil(char* buffer, std::size_t size, const char* terminator);

        /// @brief Ignores a number of bytes from the stream.
        /// @param size Number of bytes to ignore.
        void ignore(std::size_t size);
    };

    // Implementation

    template <typename T>
    requires(std::is_integral_v<T>&& std::is_signed_v<T>) inline void Stream::print(T value, std::size_t base)
    {
        this->print(static_cast<int64_t>(value), base);
    }

    template <typename T>
    requires(std::is_integral_v<T> && !std::is_signed_v<T>) inline void Stream::print(T value, std::size_t base)
    {
        this->print(static_cast<uint64_t>(value), base);
    }

    template <typename T, typename... TArgs>
    void Stream::printf(const char* fmt, T arg, TArgs... args)
    {
        for (;; ++fmt)
        {
            if (fmt[0] == '\0')
            {
                break;
            }

            if (fmt[0] == '\\')
            {
                fmt += 1;
                this->put(fmt[0]);
            }
            else if (fmt[0] == '{' && fmt[1] == '}')
            {
                this->print(arg);
                this->printf(fmt + 2, args...);
                return;
            }
            else
            {
                this->put(fmt[0]);
            }
        }
    }

    inline void Stream::printf(const char* fmt)
    {
        this->print(fmt);
    }

} // namespace cubos::core::memory
