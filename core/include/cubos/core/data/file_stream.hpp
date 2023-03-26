#pragma once

#include <utility>

#include <cubos/core/data/file.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// Represents a stream of data from/to a file.
    /// Acts as a wrapper around a specific file stream, while maintaining a reference to the file so that it
    /// can be destroyed automatically when the stream is destroyed.
    /// @tparam T The type of internal stream in the stream.
    template <typename T>
    class FileStream final : public memory::Stream
    {
    public:
        /// @param file The file which the stream is reading/writing from/to.
        /// @param mode The mode to open the file in.
        /// @param stream The stream to read/write from/to.
        FileStream(File::Handle file, File::OpenMode mode, T&& stream);
        ~FileStream() override = default;

        std::size_t read(void* buffer, std::size_t size) override;
        std::size_t write(const void* buffer, std::size_t size) override;
        std::size_t tell() const override;
        void seek(ptrdiff_t offset, memory::SeekOrigin origin) override;
        bool eof() const override;
        char peek() const override;

    private:
        File::Handle mFile;
        File::OpenMode mMode;
        T mStream;
    };

    // Implementation

    template <typename T>
    inline FileStream<T>::FileStream(File::Handle file, File::OpenMode mode, T&& stream)
        : mFile(std::move(file))
        , mMode(mode)
        , mStream(std::move(stream))
    {
    }

    template <typename T>
    inline std::size_t FileStream<T>::read(void* buffer, std::size_t size)
    {
        if (mMode != File::OpenMode::Read)
        {
            CUBOS_CRITICAL("Can't read from a file stream opened for writing.");
            abort();
        }

        return mStream.read(buffer, size);
    }

    template <typename T>
    inline std::size_t FileStream<T>::write(const void* buffer, std::size_t size)
    {
        if (mMode != File::OpenMode::Write)
        {
            CUBOS_CRITICAL("Can't write to a file stream opened for reading.");
            abort();
        }

        return mStream.write(buffer, size);
    }

    template <typename T>
    inline std::size_t FileStream<T>::tell() const
    {
        return mStream.tell();
    }

    template <typename T>
    inline void FileStream<T>::seek(ptrdiff_t offset, memory::SeekOrigin origin)
    {
        mStream.seek(offset, origin);
    }

    template <typename T>
    inline bool FileStream<T>::eof() const
    {
        return mStream.eof();
    }

    template <typename T>
    inline char FileStream<T>::peek() const
    {
        return mStream.peek();
    }
} // namespace cubos::core::data
