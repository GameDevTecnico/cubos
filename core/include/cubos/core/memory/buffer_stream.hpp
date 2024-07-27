/// @file
/// @brief Class @ref cubos::core::memory::BufferStream.
/// @ingroup core-memory

#pragma once

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::memory
{
    /// @brief Stream implementation which writes to/reads from a buffer.
    /// @ingroup core-memory
    class CUBOS_CORE_API BufferStream : public Stream
    {
    public:
        ~BufferStream() override;

        /// @brief Constructs using an existing buffer.
        /// @param buffer Buffer to read/write from.
        /// @param size Size of the buffer.
        /// @param readOnly Whether the buffer is read-only.
        BufferStream(void* buffer, std::size_t size, bool readOnly = false);

        /// @brief Constructs using a read-only buffer.
        /// @param buffer Buffer to read/write from.
        /// @param size Size of the buffer.
        BufferStream(const void* buffer, std::size_t size);

        /// @brief Constructs using a new buffer, managed internally and which grows as needed.
        /// @param size Initial size of the buffer.
        BufferStream(std::size_t size = 16);

        /// @brief Constructs a copy of another buffer stream. If the given buffer stream owns its
        /// buffer, the copy will also create its own buffer. Otherwise, it will share the buffer
        /// with the original.
        /// @param other Buffer stream to copy.
        BufferStream(const BufferStream& other);

        /// @brief Move constructs.
        /// @param other Buffer stream to move.
        BufferStream(BufferStream&& other) noexcept;

        /// @brief Gets the buffer of this stream.
        /// @return Buffer.
        const void* getBuffer() const;

        /// @brief Creates a string from the buffer of this stream.
        /// @return String.
        std::string string() const;

        // Method implementations.

        std::size_t read(void* data, std::size_t size) override;
        std::size_t write(const void* data, std::size_t size) override;
        std::size_t tell() const override;
        void seek(ptrdiff_t offset, SeekOrigin origin) override;
        bool eof() const override;
        char peek() override;

    private:
        void* mBuffer;         ///< Pointer to the buffer being written to/read from.
        std::size_t mSize;     ///< Size of the buffer.
        std::size_t mPosition; ///< Current position in the buffer.
        bool mReadOnly;        ///< Whether the buffer is read-only.
        bool mReachedEof;      ///< Whether the end of the buffer has been reached.
        bool mOwned;           ///< Whether the buffer is owned by this stream.
    };
} // namespace cubos::core::memory
