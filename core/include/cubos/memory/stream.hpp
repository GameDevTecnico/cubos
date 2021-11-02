#ifndef CUBOS_MEMORY_STREAM_HPP
#define CUBOS_MEMORY_STREAM_HPP

#include <cstdint>

namespace cubos::memory
{
    /// Abstract class for memory streams.
    class Stream
    {
    public:
        virtual ~Stream() = default;

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
    };
} // namespace cubos::memory

#endif // CUBOS_MEMORY_STREAM_HPP
