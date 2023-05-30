/// @file
/// @brief Defines the FileStream implementation of the Stream interface class.

#pragma once

#include <utility>

#include <cubos/core/data/file.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// @brief Wrapper around a specific file stream which maintains a reference to the file,
    /// preventing it from being destroyed while the stream is still in use. Should not be used
    /// directly - it is meant to be created by Archive implementations and then accessed by the
    /// user through an abstract Stream interface.
    ///
    /// @see File Archive memory::Stream
    /// @tparam T Inner stream type.
    template <typename T>
    class FileStream final : public memory::Stream
    {
    public:
        /// @param file File which the stream is reading/writing from/to.
        /// @param mode Mode to open the file in.
        /// @param stream Stream to read/write from/to.
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
        CUBOS_ASSERT(mMode == File::OpenMode::Read, "Must not read from a file stream opened for writing");
        return mStream.read(buffer, size);
    }

    template <typename T>
    inline std::size_t FileStream<T>::write(const void* buffer, std::size_t size)
    {
        CUBOS_ASSERT(mMode == File::OpenMode::Write, "Must not write to a file stream opened for reading");
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
