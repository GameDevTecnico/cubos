/// @file
/// @brief Class @ref cubos::core::data::FileSystem.
/// @ingroup core-data-fs

#pragma once

#include <cubos/core/data/fs/file.hpp>

namespace cubos::core::data
{
    /// @brief Singleton which represents the virtual file system of the engine.
    ///
    /// All filesystem operations in the engine should be done through this class or through a @ref
    /// File obtained here.
    ///
    /// @note Operations on this class expect absolute paths and are equivalent to the same
    /// operations on the root file with relative paths.
    ///
    /// @see File
    /// @ingroup core-data-fs
    class CUBOS_CORE_API FileSystem final
    {
    public:
        FileSystem() = delete;
        ~FileSystem() = delete;

        /// @brief Gets the root file of the virtual file system.
        /// @return File handle.
        static File::Handle root();

        /// @brief Mounts an archive to an absolute path.
        ///
        /// Creates any parent directories that may be necessary, but the mount point itself must
        /// not already exist.
        ///
        /// This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - a parent file in the path exists and is not a directory.
        /// - a parent directory in the path belongs to an archive.
        /// - a file already exists at the mount point.
        ///
        /// @see File::mount()
        /// @param path Absolute path to mount the archive on.
        /// @param archive Archive to mount.
        /// @return Whether the archive was successfully mounted.
        static bool mount(std::string_view path, std::unique_ptr<Archive> archive);

        /// @brief Unmounts an archive from an absolute path.
        ///
        /// Removes all of the archive's files from the virtual file system.
        ///
        /// This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - any of the files in the path do not exist.
        /// - the target path is not the mount point of an archive.
        ///
        /// @see File::unmount()
        /// @param path Path to unmount the archive from.
        /// @return Whether the archive was successfully unmounted.
        static bool unmount(std::string_view path);

        /// @brief Gets a handle to a file.
        ///
        /// Fails if the file does not exist or the path is relative or invalid.
        ///
        /// @see File::find()
        /// @param path Absolute path to the file.
        /// @return File handle or nullptr on failure.
        static File::Handle find(std::string_view path);

        /// @brief Creates a new file at the specified absolute path.
        ///
        /// The destination location must be writeable - that is, it must be under a mounted
        /// writeable archive.
        ///
        /// If a file at the specified path already exists, it is returned instead. Any parent
        /// directories that may be necessary are created.
        ///
        /// This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - a parent directory in the path does not exist and cannot be created.
        /// - a parent directory in the path already exists as a regular file.
        /// - a file of a different type already exists at the destination.
        ///
        /// @see File::create()
        /// @param path Absolute path to the file.
        /// @param directory Whether the new file should be a directory.
        /// @return File or nullptr on failure.
        static File::Handle create(std::string_view path, bool directory = false);

        /// @brief Marks a file for destruction.
        ///
        /// Although The file will only be deleted when no more references to it exist, it is
        /// immediately removed from the virtual file system.
        ///
        /// All children of the file are also marked for destruction.
        ///
        /// This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - the file does not exist.
        /// - the file is the mount point of an archive.
        /// - the file does not belong to an archive.
        /// - the file belongs to a read-only archive.
        ///
        /// @see File::destroy()
        /// @param path Absolute path of the file to destroy.
        /// @return Whether the file was successfully marked for destruction.
        static bool destroy(std::string_view path);

        /// @brief Opens a file in the virtual file system.
        ///
        /// If the file is being written to, blocks until the other threads are done with the file.
        ///
        /// This method fails on the following conditions:
        /// - `FileSystem::find(path)` fails.
        /// - `file->open(mode)` fails.
        ///
        /// @param path Absolute path of the file.
        /// @param mode Mode to open the file in.
        /// @return File stream, or nullptr if an error occurred.
        static std::unique_ptr<memory::Stream> open(std::string_view path, File::OpenMode mode);

        /// @brief Copies a file from the source path to the destination path.
        ///
        /// This method opens the source file in binary mode, reads its contents, and writes them to
        /// the destination file. If the destination file already exists, it will be overwritten.
        ///
        /// @note The source file needs to exist.
        ///
        /// @param sourcePath Absolute path of the source file.
        /// @param destinationPath Absolute path of the destination file.
        /// @return Whether the file was successfully copied.
        static bool copy(std::string_view sourcePath, std::string_view destinationPath);
    };
} // namespace cubos::core::data
