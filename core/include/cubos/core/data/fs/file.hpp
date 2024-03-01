/// @file
/// @brief Class @ref cubos::core::data::File.
/// @ingroup core-data-fs

#pragma once

#include <memory>
#include <mutex>
#include <string_view>

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    class Archive;
    class FileSystem;

    /// @brief Represents a file in the virtual file system of the engine.
    ///
    /// Instances of this class are always held through a File::Handle, which is just a smart
    /// shared pointer.
    ///
    /// Files may either be regular files or directories. Directories can have children files, but
    /// no data. Regular files are only allowed inside mounted archives, since they must have data
    /// associated with them, while the directories are just a way to organize the files.
    ///
    /// @note All operations on this class expect relative paths without leading or trailing
    /// slashes, and are equivalent to the same operations on the @ref FileSystem with absolute
    /// paths. For example, the empty path and the paths `foo/bar` and `foo` are valid relative
    /// paths, while `/foo/bar`, `foo/bar/` and `/` are not.
    ///
    /// @see FileSystem
    /// @see Archive
    /// @ingroup core-data-fs
    class CUBOS_CORE_API File final : public std::enable_shared_from_this<File>
    {
    public:
        /// @brief Handle to a file in the CUBOS. virtual file system.
        ///
        /// This is a smart shared pointer, so that files are not destroyed while being referenced.
        using Handle = std::shared_ptr<File>;

        /// @brief Possible modes to open files in.
        enum class OpenMode
        {
            Read,      ///< Open the file for reading.
            Write,     ///< Open the file for writing, overwriting any previous changes.
            ReadWrite, ///< Open the file for reading or writing data.
        };

        ~File();

        /// @brief Mounts an archive to a path relative to this file.
        ///
        /// Creates any parent directories that may be necessary, but the mount point itself must
        /// not already exist.
        ///
        /// This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - a parent file in the path exists and is not a directory.
        /// - a parent directory in the path belongs to an archive.
        /// - a file already exists at the mount point.
        ///
        /// @param path Path relative to this file to mount the archive on.
        /// @param archive Archive to mount.
        /// @return Whether the archive was successfully mounted.
        bool mount(std::string_view path, std::unique_ptr<Archive> archive);

        /// @brief Unmounts an archive from a path relative to this file.
        ///
        /// Removes all of the archive's files from the virtual file system.
        ///
        /// This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - any of the files in the path do not exist.
        /// - the target path is not the mount point for an archive.
        ///
        /// @param path Path relative to this file to unmount the archive from.
        /// @return Whether the archive was successfully unmounted.
        bool unmount(std::string_view path);

        /// @brief Gets a handle to a file relative to this file.
        ///
        /// Fails if the file does not exist or if the path is absolute or invalid.
        ///
        /// @param path Path relative to this file to get the file from.
        /// @return Handle to the file, or nullptr on failure.
        Handle find(std::string_view path);

        /// @brief Creates a new file on a path relative to this file.
        ///
        /// The destination location must be writeable - that is, it must be under a mounted
        /// writeable archive. If a file at the given path already exists, it is returned instead.
        /// Any parent directories that may be necessary are created.
        ///
        /// This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - a parent directory in the path does not exist and cannot be created.
        /// - a parent directory in the path already exists as a regular file.
        /// - a file of a different type already exists at the destination.
        ///
        /// @param path Relative path to the file.
        /// @param directory Whether the new file should be a directory.
        /// @return Handle to the file, or nullptr on failure.
        Handle create(std::string_view path, bool directory = false);

        /// @brief Marks this file for destruction.
        ///
        /// Although the file will only be deleted when no more references to it exist, it is
        /// immediately removed from the virtual file system and this method is called recursively
        /// on all its children.
        ///
        /// This method fails on the following conditions:
        /// - this file is the mount point of an archive.
        /// - this file does not belong to an archive.
        /// - this file belongs to a read-only archive.
        ///
        /// @return Whether the file was successfully marked for destruction.
        bool destroy();

        /// @brief Opens this file for reading or writing.
        ///
        /// If the file is being written to, blocks until the other threads are done with the file.
        ///
        /// This method fails on the following conditions:
        /// - this file is a directory.
        /// - this file belongs to a read-only archive and the mode is not @ref OpenMode::Read.
        ///
        /// @param mode Mode to open the file in.
        /// @return Handle to a file stream, or nullptr on failure.
        std::unique_ptr<memory::Stream> open(OpenMode mode);

        /// @brief Gets the name of this file.
        /// @return Name of this file.
        std::string_view name() const;

        /// @brief Gets the absolute path of this file.
        /// @return Absolute path of this file.
        std::string_view path() const;

        /// @brief Checks whether this file is a directory.
        /// @return Whether this file is a directory.
        bool directory() const;

        /// @brief Gets the archive this file is in.
        /// @return Archive this file is in, or nullptr if the file is not in an archive.
        const std::shared_ptr<Archive>& archive() const;

        /// @brief Gets the identifier of this file on its archive.
        /// @return Identifier of this file on its archive, or 0 if it is not in an archive.
        std::size_t id() const;

        /// @brief Gets the parent directory of this file.
        /// @return File's parent directory, or nullptr if there isn't one.
        Handle parent() const;

        /// @brief Gets the next sibling of this file.
        /// @return Next sibling, or nullptr if this file is the last child of its parent.
        Handle sibling() const;

        /// @brief Gets the first child of this file.
        /// @return First child, or nullptr if this file is not a directory or if it is empty.
        Handle child() const;

    private:
        friend FileSystem;

        /// @brief Constructor for directories outside archives.
        /// @param parent Parent file handle.
        /// @param name Name of the directory.
        File(Handle parent, std::string_view name);

        /// @brief Constructor for archive files.
        /// @param parent Parent file handle.
        /// @param archive Archive the file belongs to.
        /// @param id Identifier of the file in the archive.
        File(Handle parent, std::shared_ptr<Archive> archive, std::size_t id);

        /// @brief Constructor for archive roots.
        /// @param parent Parent file handle.
        /// @param archive Archive the file belongs to.
        /// @param name Name of the archive.
        File(Handle parent, std::shared_ptr<Archive> archive, std::string_view name);

        /// @brief Adds a child file to this file.
        /// @param child Child file to add.
        void addChild(const File::Handle& child);

        /// @brief Removes a child file from this file.
        /// @param child Child file to remove.
        void removeChild(const File::Handle& child);

        /// @brief Finds a child file in this file.
        /// @param name Name of the child file to find.
        /// @return Handle to the child file, or nullptr if the child file does not exist.
        File::Handle findChild(std::string_view name) const;

        /// @brief Recursively add an archive's files to the virtual file system.
        ///
        /// Called after the archive has been mounted.
        void addArchive();

        /// @brief Recursively removes the archive's files from the virtual file system.
        ///
        /// Called after the archive is unmounted.
        void removeArchive();

        /// @brief Recursively destroys a file and its children from the virtual file system.
        ///
        /// Called after the file is destroyed.
        void destroyRecursive();

        std::string mPath;       ///< Path of this file.
        std::string mName;       ///< Name of this file.
        bool mDirectory = false; ///< Whether this file is a directory.

        std::shared_ptr<Archive> mArchive = nullptr; ///< Archive this file belongs to.
        std::size_t mId = 0;                         ///< Id of this file in its archive.

        Handle mParent = nullptr;  ///< Parent file handle.
        Handle mSibling = nullptr; ///< Next sibling file handle.
        Handle mChild = nullptr;   ///< First child file handle.

        bool mDestroyed = false; ///< Whether this file has been marked for deletion.

        mutable std::mutex mMutex; ///< Mutex used to synchronize changing properties of this file.
    };
} // namespace cubos::core::data
