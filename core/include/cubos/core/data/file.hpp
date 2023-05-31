/// @file
/// @brief Defines the File class.

#pragma once

#include <memory>
#include <mutex>
#include <string_view>

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    class Archive;
    class FileSystem;

    /// @brief Represents a file in the virtual file system of the engine. Should always be held
    /// through a File::Handle, which is just a smart shared pointer. All operations on files
    /// through this class are done with relative paths, and are equivalent to the same operations
    /// on FileSystem, with absolute paths.
    ///
    /// @details Files may be directories, in which case they can have children files, but no data.
    /// Regular files are only allowed inside archives, since they must have data associated with
    /// them, while the directories are just a way to organize the files.
    ///
    /// Archives are mounted to files, such that the file and all its children are in the archive.
    /// Archives can be either single-file archives, in which case their mount point is the file
    /// itself, or they can be directories, in which case their mount point is a directory.
    ///
    /// The FileSystem object should be used to access files from absolute paths - all operations
    /// on files are done with relative paths, which must not have leading or trailing slashes.
    /// For example, the following paths are valid relative paths: "foo/bar", "foo", "".
    /// The following paths are invalid relative paths: "/foo/bar", "foo/bar/", "/".
    ///
    /// @see FileSystem Archive
    class File final : public std::enable_shared_from_this<File>
    {
    public:
        /// @brief Smart shared pointer, used to prevent files from being deleted while it is still
        /// in use.
        using Handle = std::shared_ptr<File>;

        /// @brief Possible modes to open files in.
        enum class OpenMode
        {
            Read,      ///< Open the file for reading.
            Write,     ///< Open the file for writing, overwriting any previous changes.
            ReadWrite, ///< Open the file for reading or writing data.
        };

        ~File();

        /// @brief Mounts an archive to a path relative to this file. Creates any parent
        /// directories that may be necessary, but the mount point itself must not already exist.
        ///
        /// @details This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - a parent file in the path exists and is not a directory.
        /// - a parent directory in the path belongs to an archive.
        /// - a file already exists at the mount point.
        ///
        /// @param path Path relative to this file to mount the archive on.
        /// @param archive Archive to mount.
        /// @return Whether the archive was successfully mounted.
        bool mount(std::string_view path, std::unique_ptr<Archive> archive);

        /// @brief Unmounts an archive from a path relative to this file. Removes all of the
        /// archive's files from the virtual file system.
        ///
        /// @details This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - any of the files in the path do not exist.
        /// - the target path is not the mount point for an archive.
        ///
        /// @param path Path relative to this file to unmount the archive from.
        /// @return Whether the archive was successfully unmounted.
        bool unmount(std::string_view path);

        /// @brief Gets a handle to a file relative to this file. Fails if the file does not exist
        /// or if the path is absolute or invalid.
        /// @param path Path relative to this file to get the file from.
        /// @return A handle to the file, or nullptr if the search failed.
        Handle find(std::string_view path);

        /// @brief Creates a new file on a path relative to this file. The destination location
        /// must be writeable - that is, it must be under a mounted writeable archive. If a file at
        /// the given path already exists, it is returned instead. Any parent directories that may
        /// be necessary are created.
        ///
        /// @details This method fails on the following conditions:
        /// - path is absolute or invalid.
        /// - a parent directory in the path does not exist and cannot be created.
        /// - a parent directory in the path already exists as a regular file.
        /// - a file of a different type already exists at the destination.
        ///
        /// @param path The relative path to the file.
        /// @param directory Whether the new file should be a directory.
        /// @return A handle to the file, or nullptr if the creation failed.
        Handle create(std::string_view path, bool directory = false);

        /// @brief Marks this file for destruction. The file will only be deleted when no more
        /// references to it exist. The file is immediatelly removed from its parent directory and
        /// this method is called recursively on all its children.
        ///
        /// @details This method fails on the following conditions:
        /// - this file is the mount point of an archive.
        /// - this file does not belong to an archive.
        /// - this file belongs to a read-only archive.
        ///
        /// The reason this method does not immediately delete the file is to prevent
        /// files from being deleted while they are open. Since files are removed from the
        /// tree immediately, it is no longer possible to reach them, but there may be other
        /// threads operating on them. After every thread is done with the file, it is deleted.
        ///
        /// @return Whether the file was successfully marked for destruction.
        bool destroy();

        /// @brief Opens this file for reading or writing. If the file is being written to, blocks
        /// until the other threads are done with the file.
        ///
        /// @details This method fails on the following conditions:
        /// - this file is a directory.
        /// - this file belongs to a read-only archive and the mode is not OpenMode::Read.
        ///
        /// If the archive where the file is is read-only, or if the file is a directory, nullptr is returned.
        /// @param mode The mode to open the file in.
        /// @return A handle to a file stream, or nullptr if the file could not be opened.
        std::unique_ptr<memory::Stream> open(OpenMode mode);

        /// @return The name of this file.
        std::string_view name() const;

        /// @return The absolute path of this file.
        std::string_view path() const;

        /// @return Whether this file is a directory.
        bool directory() const;

        /// @return The archive this file is in, or nullptr if the file is not in an archive.
        const std::shared_ptr<Archive>& archive() const;

        /// @return The identifier of this file on its archive, or 0 if it is not in an archive.
        std::size_t id() const;

        /// @return The file's parent directory, or nullptr if there isn't one.
        Handle parent() const;

        /// @return The next sibling, or nullptr if this file is the last child of its parent.
        Handle sibling() const;

        /// @return The first child, or nullptr if this file is not a directory or if it is empty.
        Handle child() const;

    private:
        friend FileSystem;

        /// @brief Construtor for directories outside archives.
        File(Handle parent, std::string_view name);

        /// @brief Construtor for archive files.
        File(Handle parent, std::shared_ptr<Archive> archive, std::size_t id);

        /// @brief Constructor for archive roots.
        File(Handle parent, std::shared_ptr<Archive> archive, std::string_view name);

        /// Adds a child file to this file.
        /// @param child The child file to add.
        void addChild(const File::Handle& child);

        /// Removes a child file from this file.
        /// @param child The child file to remove.
        void removeChild(const File::Handle& child);

        /// Finds a child file in this file.
        /// @param name The name of the child file to find.
        /// @return A handle to the child file, or nullptr if the child file does not exist.
        File::Handle findChild(std::string_view name) const;

        /// Recursively add an archive's files to the virtual file system.
        /// Called after the archive has been mounted.
        void addArchive();

        /// Recursively removes the archive's files from the virtual file system.
        /// Called after the archive is unmounted.
        void removeArchive();

        /// Recursively destroys a file and its children from the virtual file system.
        /// Called after the file is destroyed.
        void destroyRecursive();

        std::string mPath;       ///< The path of this file.
        std::string mName;       ///< The name of this file.
        bool mDirectory = false; ///< Whether this file is a directory.

        std::shared_ptr<Archive> mArchive = nullptr; ///< The archive this file belongs to.
        std::size_t mId = 0;                         ///< The id of this file in its archive.

        Handle mParent = nullptr;  ///< The parent file handle.
        Handle mSibling = nullptr; ///< The next sibling file handle.
        Handle mChild = nullptr;   ///< The first child file handle.

        bool mDestroyed = false; ///< Whether this file has been marked for deletion.

        mutable std::mutex mMutex; ///< The mutex used to synchronize changing properties of this file.
    };
} // namespace cubos::core::data
