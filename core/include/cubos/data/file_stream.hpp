#ifndef CUBOS_DATA_FILE_STREAM_HPP
#define CUBOS_DATA_FILE_STREAM_HPP

#include <cubos/data/file.hpp>
#include <cubos/memory/stream.hpp>
#include <cubos/log.hpp>

namespace cubos::data
{
    /// Represents a stream of data from/to a file.
    /// Acts as a wrapper around a specific file stream, while maintaining a reference to the file so that it
    /// can be destroyed automatically when the stream is destroyed.
    /// @tparam T The type of internal stream in the stream.
    template <typename T> class FileStream final : public memory::Stream
    {
    public:
        /// @param file The file which the stream is reading/writing from/to.
        /// @param mode The mode to open the file in.
        /// @param stream The stream to read/write from/to.
        FileStream(File::Handle file, File::OpenMode mode, T&& stream);
        virtual ~FileStream() override = default;

        virtual size_t read(void* buffer, size_t size) override;
        virtual size_t write(const void* buffer, size_t size) override;
        virtual size_t tell() const override;
        virtual void seek(int64_t offset, memory::SeekOrigin origin) override;
        virtual bool eof() const override;
        virtual char peek() const override;

    private:
        File::Handle file;
        File::OpenMode mode;
        T stream;
    };

    // Implementation

    template <typename T>
    inline FileStream<T>::FileStream(File::Handle file, File::OpenMode mode, T&& stream)
        : file(file), mode(mode), stream(std::move(stream))
    {
    }

    template <typename T> inline size_t FileStream<T>::read(void* buffer, size_t size)
    {
        if (this->mode != File::OpenMode::Read)
        {
            logError("Attempted to read from a file stream opened for writing.");
            abort();
        }

        return stream.read(buffer, size);
    }

    template <typename T> inline size_t FileStream<T>::write(const void* buffer, size_t size)
    {
        if (this->mode != File::OpenMode::Write)
        {
            logError("Attempted to write to a file stream opened for reading.");
            abort();
        }

        return stream.write(buffer, size);
    }

    template <typename T> inline size_t FileStream<T>::tell() const
    {
        return stream.tell();
    }

    template <typename T> inline void FileStream<T>::seek(int64_t offset, memory::SeekOrigin origin)
    {
        stream.seek(offset, origin);
    }

    template <typename T> inline bool FileStream<T>::eof() const
    {
        return stream.eof();
    }

    template <typename T> inline char FileStream<T>::peek() const
    {
        return stream.peek();
    }
} // namespace cubos::data

#endif // CUBOS_DATA_FILE_STREAM_HPP
