#ifndef CUBOS_MEMORY_STD_STREAM_HPP
#define CUBOS_MEMORY_STD_STREAM_HPP

#include <cubos/memory/stream.hpp>

#include <cstdio>

namespace cubos::memory
{
    /// Stream implementation for stdio.
    class StdStream : public Stream
    {
    public:
        /// @param file Stdio file to read/write from.
        /// @param close Should the file be closed when this stream is destructed?
        StdStream(FILE* file, bool close = false);
        StdStream(StdStream&&);
        virtual ~StdStream() override;

        virtual size_t read(void* data, size_t size) override;
        virtual size_t write(const void* data, size_t size) override;
        virtual size_t tell() const override;
        virtual void seek(int64_t offset, SeekOrigin origin) override;
        virtual bool eof() const override;
        virtual char peek() const override;

    private:
        FILE* file; ///< Stdio file to read/write from.
        bool close; ///< Should the file be closed when this stream is destructed?
    };
} // namespace cubos::memory

#endif // CUBOS_MEMORY_STD_STREAM_HPP
