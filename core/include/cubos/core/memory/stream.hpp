#ifndef CUBOS_CORE_MEMORY_STREAM_HPP
#define CUBOS_CORE_MEMORY_STREAM_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace cubos::core::memory
{
    /// Seek origin.
    enum class SeekOrigin
    {
        Begin,
        Current,
        End
    };

    /// Abstract class for memory streams.
    class Stream
    {
    public:
        Stream() = default;
        Stream(Stream&&) = default;
        Stream(const Stream&) = delete;
        virtual ~Stream() = default;

        static Stream& stdIn;  ///< Stream wrapper for stdin.
        static Stream& stdOut; ///< Stream wrapper for stdout.
        static Stream& stdErr; ///< Stream wrapper for stderr.

        /// Reads data from the stream.
        /// @param data The buffer to read data into.
        /// @param size The size of the buffer.
        /// @return The number of bytes read.
        virtual size_t read(void* data, size_t size) = 0;

        /// Writes data to the stream.
        /// @param data The buffer to write data from.
        /// @param size The size of the buffer.
        /// @return The number of bytes written.
        virtual size_t write(const void* data, size_t size) = 0;

        /// Gets the current position in the stream.
        /// @return The current position in the stream, or SIZE_MAX if the position is unknown.
        virtual size_t tell() const = 0;

        /// Seeks to a position in the stream.
        /// @param offset The offset to seek to.
        /// @param origin The origin of the offset.
        virtual void seek(ptrdiff_t offset, SeekOrigin origin) = 0;

        /// Checks if the stream still has content to read.
        /// @return True if the stream has no more content to read, false otherwise.
        virtual bool eof() const = 0;

        /// Peeks one byte from the stream.
        /// @return The byte peeked.
        virtual char peek() const = 0;

        /// Gets one byte from the stream.
        /// @return The byte read.
        char get();

        /// Puts one byte into the stream.
        /// @param c The byte to put.
        void put(char c);

        /// Prints a signed integer to the stream.
        /// @tparam T The type of the integer.
        /// @param value The value to print.
        /// @param base The base to use.
        template <typename T>
        requires(std::is_integral_v<T>&& std::is_signed_v<T>) void print(T value, size_t base = 10);

        /// Prints a signed integer to the stream.
        /// @tparam T The type of the integer.
        /// @param value The value to print.
        /// @param base The base to use.
        template <typename T>
        requires(std::is_integral_v<T> && !std::is_signed_v<T>) inline void print(T value, size_t base = 10);

        /// Prints a 64 bit signed integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(int64_t value, size_t base = 10);

        /// Prints a 64 bit unsigned integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(uint64_t value, size_t base = 10);

        /// Prints a float to the stream.
        /// @param value The value to print.
        /// @param decimalPlaces The number of decimal places to print.
        void print(float value, size_t decimalPlaces = 4);

        /// Prints a double to the stream.
        /// @param value The value to print.
        /// @param decimalPlaces The number of decimal places to print.
        void print(double value, size_t decimalPlaces = 4);

        /// Prints a string to the stream.
        /// @param str The value to print.
        void print(const char* str);

        /// Prints a string to the stream.
        /// @param str The string to print.
        /// @param size The size of the string.
        void print(const char* str, size_t size);

        /// Prints a string to the stream.
        /// @param str The value to print.
        void print(const std::string& str);

        /// Prints a formatted string the stream.
        /// Example usage:
        ///
        ///     stream.printf("Hello, {}!\n", "world");
        ///     stream.printf("{} + {} = {}\n", 1, 2, 3);
        ///     stream.printf("\\{} {}\n", 1, 2); // This will print "{} 2"
        ///
        /// @tparam T Type of the first argument.
        /// @tparam TArgs The types of the remaining arguments.
        /// @param arg First argument to print.
        /// @param args The arguments to print.
        template <typename T, typename... TArgs>
        void printf(const char* fmt, T arg, TArgs... args);

        /// Formatted print recursion tail function.
        /// @param fmt The remainder format string.
        void printf(const char* fmt);

        /// Parses a 8 bit signed integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(int8_t& value, size_t base = 10);

        /// Parses a 16 bit signed integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(int16_t& value, size_t base = 10);

        /// Parses a 32 bit signed integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(int32_t& value, size_t base = 10);

        /// Parses a 64 bit signed integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(int64_t& value, size_t base = 10);

        /// Parses a 8 bit unsigned integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(uint8_t& value, size_t base = 10);

        /// Parses a 16 bit unsigned integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(uint16_t& value, size_t base = 10);

        /// Parses a 32 bit unsigned integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(uint32_t& value, size_t base = 10);

        /// Parses a 64 bit unsigned integer from the stream.
        /// @param value Parsed value.
        /// @param base The base to use.
        void parse(uint64_t& value, size_t base = 10);

        /// Parses a float from the stream.
        /// @param value Parsed value.
        void parse(float& value);

        /// Parses a double from the stream.
        /// @param value Parsed value.
        void parse(double& value);

        /// Reads a string from the stream until the terminator (or '\0') is found.
        /// If terminator is nullptr reads a string until '\0' is found.
        /// @param str The string to read.
        /// @param terminator The terminator to use.
        void readUntil(std::string& str, const char* terminator);

        /// Reads from the stream until the terminator (or '\0') is found.
        /// @param buffer The buffer to read into.
        /// @param size The size of the buffer.
        /// @param terminator The terminator to use.
        /// @return The number of bytes read.
        size_t readUntil(char* buffer, size_t size, const char* terminator);

        /// Ignores a number of bytes from the stream.
        /// @param size The number of bytes to ignore.
        void ignore(size_t size);
    };

    // Implementation

    template <typename T>
    requires(std::is_integral_v<T>&& std::is_signed_v<T>) inline void Stream::print(T value, size_t base)
    {
        this->print(static_cast<int64_t>(value), base);
    }

    template <typename T>
    requires(std::is_integral_v<T> && !std::is_signed_v<T>) inline void Stream::print(T value, size_t base)
    {
        this->print(static_cast<uint64_t>(value), base);
    }

    template <typename T, typename... TArgs>
    void Stream::printf(const char* fmt, T arg, TArgs... args)
    {
        for (;; ++fmt)
        {
            if (fmt[0] == '\0')
                break;
            else if (fmt[0] == '\\')
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
                this->put(fmt[0]);
        }
    }

    inline void Stream::printf(const char* fmt)
    {
        this->print(fmt);
    }

} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_STREAM_HPP
