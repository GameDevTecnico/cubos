#ifndef CUBOS_MEMORY_STREAM_HPP
#define CUBOS_MEMORY_STREAM_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace cubos::memory
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
        /// @return The current position in the stream.
        virtual size_t tell() const = 0;

        /// Seeks to a position in the stream.
        /// @param offset The offset to seek to.
        /// @param origin The origin of the offset.
        virtual void seek(int64_t offset, SeekOrigin origin) = 0;

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

        /// Prints a 8 bit signed integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(int8_t value, size_t base = 10);

        /// Prints a 16 bit signed integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(int16_t value, size_t base = 10);

        /// Prints a 32 bit signed integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(int32_t value, size_t base = 10);

        /// Prints a 64 bit signed integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(int64_t value, size_t base = 10);

        /// Prints a 8 bit unsigned integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(uint8_t value, size_t base = 10);

        /// Prints a 16 bit unsigned integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(uint16_t value, size_t base = 10);

        /// Prints a 32 bit unsigned integer to the stream.
        /// @param value The value to print.
        /// @param base The base to use.
        void print(uint32_t value, size_t base = 10);

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
    };
} // namespace cubos::memory

#endif // CUBOS_MEMORY_STREAM_HPP
