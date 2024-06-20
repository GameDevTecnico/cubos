/// @file
/// @brief Class @ref cubos::core::data::FileStream.
/// @ingroup core-data-fs

#pragma once

#include <utility>

#include <cubos/core/data/fs/file.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/tel/logging.hpp>

namespace cubos::core::data
{
    /// @brief Wrapper around an implementation-specific file stream which keeps the file alive and
    /// does some sanity checks.
    ///
    /// @note This is not mean to be used by the user directly. It is meant to be created
    /// internally by @ref Archive implementations and returned as a pointer to a @ref
    /// memory::Stream.
    ///
    /// The file is kept alive by storing a handle to it, which prevents it from being destroyed
    /// as long as the stream is alive.
    ///
    /// @see File
    /// @see Archive
    /// @tparam T Inner implementation-specific stream type.
    /// @ingroup core-data-fs
    template <typename T>
    class FileStream final : public memory::Stream
    {
    public:
        /// @brief Constructs.
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
        char peek() override;

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
        CUBOS_ASSERT(mMode != File::OpenMode::Write, "Must not read from a file stream opened for writing");
        return mStream.read(buffer, size);
    }

    template <typename T>
    inline std::size_t FileStream<T>::write(const void* buffer, std::size_t size)
    {
        CUBOS_ASSERT(mMode != File::OpenMode::Read, "Must not write to a file stream opened for reading");
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
    inline char FileStream<T>::peek()
    {
        return mStream.peek();
    }
} // namespace cubos::core::data
