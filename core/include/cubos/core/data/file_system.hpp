#pragma once

#include <cubos/core/data/file.hpp>

namespace cubos::core::data
{
    /// @brief Singleton class which represents the virtual file system of the engine.
    /// All operations on files through this class are done with absolute paths, and are equivalent
    /// to the same operations on the root file, with relative paths.
    ///
    /// @see File
    class FileSystem final
    {
    public:
        FileSystem() = delete;
        ~FileSystem() = delete;

        /// @return The root file of the virtual file system.
        static File::Handle root();

        /// @brief Mounts an archive to an absolute path. The directory which contains the path
        /// must exist, but the path itself must not (unless it is the root).
        ///
        /// @details This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - a parent directory in the path does not exist.
        /// - a non-root file already exists at the mount point.
        /// - the mount point is the root file, but the archive is not a directory archive.
        /// - the mount point is the root file, but another archive is already mounted to the root.
        /// - the mount point is the root file, but the root is not empty.
        ///
        /// @see File::mount
        /// @param path Absolute path to mount the archive on.
        /// @param archive Archive to mount.
        /// @return Whether the archive was successfully mounted.
        static bool mount(std::string_view path, std::unique_ptr<Archive> archive);

        /// @brief Unmounts an archive from an absolute path.
        ///
        /// @details This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - any of the files in the path do not exist.
        /// - the target path is not the mount point of an archive.
        ///
        /// @see File::unmount
        /// @param path Path to unmount the archive from.
        /// @return Whether the archive was successfully unmounted.
        static bool unmount(std::string_view path);

        /// @brief Gets a handle to a file. Fails if the file does not exist or the path is relative
        /// or invalid.
        /// @see File::find
        /// @param path Absolute path to the file.
        /// @return The file or nullptr if the search failed.
        static File::Handle find(std::string_view path);

        /// @brief Creates a new file at the specified absolute path. The destination location
        /// must be writeable - that is, it must be under a mounted writeable archive. If a file at
        /// the specified path already exists, it is returned instead. Any parent directories that
        /// may be necessary are created.
        ///
        /// @details This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - a parent directory in the path does not exist and cannot be created.
        /// - a parent directory in the path already exists as a regular file.
        /// - a file of a different type already exists at the destination.
        ///
        /// @see File::create
        /// @param path The absolute path to the file.
        /// @param directory Whether the new file should be a directory.
        /// @return The file or nullptr if the creation failed.
        static File::Handle create(std::string_view path, bool directory = false);

        /// @brief Marks a file for destruction. The file will only be deleted when no more
        /// references to it exist. The file is immediately removed from its parent directory and
        /// any children of the file are also marked for destruction.
        ///
        /// @details This method fails on the following conditions:
        /// - path is relative or invalid.
        /// - the file does not exist.
        /// - the file is the root file.
        /// - the file is the mount point of an archive.
        /// - the file belongs to a read-only archive.
        ///
        /// @see File::destroy
        /// @param path Absolute path of the file to destroy.
        /// @return Whether the file was successfully marked for destruction.
        static bool destroy(std::string_view path);

        /// Opens a file in the virtual file system.
        ///
        /// @details This method fails on the following conditions:
        /// - FileSystem::find(path) fails.
        /// - file->open(mode) fails.
        ///
        /// @param path Absolute path of the file.
        /// @param mode Mode to open the file in.
        /// @return The file stream, or nullptr if an error occurred.
        static std::unique_ptr<memory::Stream> open(std::string_view path, File::OpenMode mode);
    };
} // namespace cubos::core::data
