#ifndef CUBOS_CORE_MEMORY_STD_STREAM_HPP
#define CUBOS_CORE_MEMORY_STD_STREAM_HPP

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
        StdStream(StdStream&&) noexcept;
        virtual ~StdStream() override;

        virtual std::size_t read(void* data, std::size_t size) override;
        virtual std::size_t write(const void* data, std::size_t size) override;
        virtual std::size_t tell() const override;
        virtual void seek(ptrdiff_t offset, SeekOrigin origin) override;
        virtual bool eof() const override;
        virtual char peek() const override;

    private:
        FILE* file; ///< Stdio file to read/write from.
        bool close; ///< Should the file be closed when this stream is destructed?
    };
} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_STD_STREAM_HPP
