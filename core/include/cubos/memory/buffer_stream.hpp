#ifndef CUBOS_CORE_MEMORY_BUFFER_STREAM_HPP
#define CUBOS_CORE_MEMORY_BUFFER_STREAM_HPP

#include <cubos/memory/stream.hpp>

namespace cubos::core::memory
{
    /// Implements a stream that writes to/reads from a buffer.
    class BufferStream : public Stream
    {
    public:
        /// @param buffer The buffer to read/write from.
        /// @param size The size of the buffer.
        BufferStream(void* buffer, size_t size, bool readOnly = false);

        /// @param buffer The buffer to read/write from.
        /// @param size The size of the buffer.
        BufferStream(const void* buffer, size_t size);

        BufferStream(BufferStream&&);

        virtual size_t read(void* data, size_t size) override;
        virtual size_t write(const void* data, size_t size) override;
        virtual size_t tell() const override;
        virtual void seek(int64_t offset, SeekOrigin origin) override;
        virtual bool eof() const override;
        virtual char peek() const override;

    private:
        void* buffer;    ///< Pointer to the buffer being written to/read from.
        size_t size;     ///< Size of the buffer.
        size_t position; ///< Current position in the buffer.
        bool readOnly;   ///< Whether the buffer is read-only.
        bool reachedEof; ///< Whether the end of the buffer has been reached.
    };
} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_BUFFER_STREAM_HPP
