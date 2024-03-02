/// @file
/// @brief Class @ref cubos::core::memory::StandardStream.
/// @ingroup core-memory

#pragma once

#include <cstdio>

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::memory
{
    /// @brief Stream implementation which wraps a `libc` file pointer.
    /// @ingroup core-memory
    class CUBOS_CORE_API StandardStream : public Stream
    {
    public:
        ~StandardStream() override;

        /// @brief Constructs.
        /// @param file File to read/write from.
        /// @param close Should the file be closed when this stream is destructed?
        StandardStream(FILE* file, bool close = false);

        /// @brief Move constructs.
        /// @param other Moved stream.
        StandardStream(StandardStream&& other) noexcept;

        std::size_t read(void* data, std::size_t size) override;
        std::size_t write(const void* data, std::size_t size) override;
        std::size_t tell() const override;
        void seek(ptrdiff_t offset, SeekOrigin origin) override;
        bool eof() const override;
        char peek() const override;

    private:
        FILE* mFile; ///< File to read/write from.
        bool mClose; ///< Should the file be closed when this stream is destructed?
    };
} // namespace cubos::core::memory
