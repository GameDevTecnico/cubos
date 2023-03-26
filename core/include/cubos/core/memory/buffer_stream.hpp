#pragma once

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::memory
{
    /// Implements a stream that writes to/reads from a buffer.
    class BufferStream : public Stream
    {
    public:
        /// @param buffer The buffer to read/write from.
        /// @param size The size of the buffer.
        BufferStream(void* buffer, std::size_t size, bool readOnly = false);

        /// @param buffer The buffer to read/write from.
        /// @param size The size of the buffer.
        BufferStream(const void* buffer, std::size_t size);

        /// Initializes a buffer stream with a new buffer, initially of the given size.
        /// When initialized this way, the buffer stream will own the buffer and will delete it when it is destroyed.
        /// It will expand the buffer when needed.
        /// @param size The size of the buffer.
        BufferStream(std::size_t size = 16);

        ~BufferStream() override;
        BufferStream(const BufferStream& /*other*/);
        BufferStream(BufferStream&& /*other*/) noexcept;

        /// Gets the buffer of this stream.
        const void* getBuffer() const;

        // Method implementations.

        std::size_t read(void* data, std::size_t size) override;
        std::size_t write(const void* data, std::size_t size) override;
        std::size_t tell() const override;
        void seek(ptrdiff_t offset, SeekOrigin origin) override;
        bool eof() const override;
        char peek() const override;

    private:
        void* mBuffer;         ///< Pointer to the buffer being written to/read from.
        std::size_t mSize;     ///< Size of the buffer.
        std::size_t mPosition; ///< Current position in the buffer.
        bool mReadOnly;        ///< Whether the buffer is read-only.
        bool mReachedEof;      ///< Whether the end of the buffer has been reached.
        bool mOwned;           ///< Whether the buffer is owned by this stream.
    };
} // namespace cubos::core::memory
