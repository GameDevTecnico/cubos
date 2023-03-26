#pragma once

#include <cstdio>

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::memory
{
    /// Stream implementation for stdio.
    class StdStream : public Stream
    {
    public:
        /// @param file Stdio file to read/write from.
        /// @param close Should the file be closed when this stream is destructed?
        StdStream(FILE* file, bool close = false);
        StdStream(StdStream&& /*other*/) noexcept;
        ~StdStream() override;

        std::size_t read(void* data, std::size_t size) override;
        std::size_t write(const void* data, std::size_t size) override;
        std::size_t tell() const override;
        void seek(ptrdiff_t offset, SeekOrigin origin) override;
        bool eof() const override;
        char peek() const override;

    private:
        FILE* mFile; ///< Stdio file to read/write from.
        bool mClose; ///< Should the file be closed when this stream is destructed?
    };
} // namespace cubos::core::memory
