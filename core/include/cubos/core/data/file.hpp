#ifndef CUBOS_CORE_DATA_FILE_HPP
#define CUBOS_CORE_DATA_FILE_HPP

#include <memory>
#include <mutex>
#include <string_view>

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    class Archive;
    class FileSystem;

    /// Represents a file in the virtual file system of the engine.
    /// The virtual file system is a tree of files where you can mount archives.
    /// Examples of possible archive types are a simple single file archive or a zipped archive.
    /// Files which are not in an archive must be directories.
    ///
    /// Usage example:
    ///     File::mount("/", ...);
    ///     File::mount("/assets", ...);
    ///     ...
    ///     auto settingsStream = root->find("./settings.yaml")->open(File::OpenMode::Read);
    ///     ...
    ///     auto cubeStream = root->find("/assets/models/cube.obj")->open(File::OpenMode::Read);
    ///
    /// Paths must not have a trailing slash.
    class File final : public std::enable_shared_from_this<File>
    {
    public:
        /// Handle to a file. This is a smart shared pointer, so that the file won't be deleted while it is still in
        /// use.
        using Handle = std::shared_ptr<File>;

        /// The possible modes to open files in.
        enum class OpenMode
        {
            Read,  ///< Open the file for reading.
            Write, ///< Open the file for writing.
        };

        ~File();

        /// Mounts an archive to a path relative to this file in the virtual file system of the engine.
        /// The directory which contains the path must exist, but the path itself must not (unless it is the root).
        /// Archives can be mounted to the root path ("/") or to any other path.
        ///
        /// If the path isn't relative, or if the parent directory of the path doesn't exist, or if there's already an
        /// archive mounted at the path, abort() is called.
        /// @param path The path to mount the archive to.
        /// @param archive The archive to mount.
        void mount(std::string_view path, std::shared_ptr<Archive> archive);

        /// Unmounts an archive from a path relative to this file in the virtual file system of the engine.
        /// If no archive is mounted at the given path, nothing is done.
        /// @param path The path to unmount the archive from.
        void unmount(std::string_view path);

        /// Searches for a file relative to this file in the virtual file system.
        /// @param path The path to the file (relative to this file).
        /// @return A handle to the file, or nullptr if the file does not exist or if the path is not relative.
        Handle find(std::string_view path);

        /// Creates a new file relative to this file in the virtual file system. The given path must be within a mounted
        /// writable archive. If the file already exists, or if the path is absolute, or if the directory doesn't exist,
        /// the function returns nullptr.
        /// @param path The relative path to the file.
        /// @param directory Whether the file is a directory.
        /// @return A handle to the file, or nullptr if the creation failed.
        Handle create(std::string_view path, bool directory = false);

        /// Marks this file for destruction. The file will be deleted when no more references to it are held.
        /// All children of this file will be marked for deletion as well.
        /// @return True if the file was marked for destroyed, false if it couldn't be destroyed.
        bool destroy();

        /// Opens this file for reading or writing.
        /// If the archive where the file is is read-only, or if the file is a directory, nullptr is returned.
        /// @param mode The mode to open the file in.
        /// @return A handle to a file stream, or nullptr if the file could not be opened.
        std::unique_ptr<memory::Stream> open(OpenMode mode);

        /// Gets the name of this file.
        std::string_view getName() const;

        /// Gets the path of this file.
        std::string_view getPath() const;

        /// Checks if this file is a directory.
        bool isDirectory() const;

        /// Gets the archive this file is in.
        /// @return The archive this file is in, or nullptr if the file is not in an archive.
        std::shared_ptr<Archive> getArchive() const;

        /// Gets the identifier of this file, used to identify the file in its archive.
        /// @return The identifier of this file, or 0 if the file is not in an archive.
        size_t getId() const;

        /// Gets the parent directory of this file.
        /// @return A handle to the parent directory, or nullptr if this file is the root file.
        Handle getParent() const;

        /// Gets the next sibling of this file.
        /// @return A handle to the next sibling, or nullptr if this file is the last child of its parent.
        Handle getSibling() const;

        /// Gets the first child of this directory.
        /// @return A handle to the first child, or nullptr if this file is not a directory or if it is empty.
        Handle getChild() const;

    private:
        friend FileSystem;

        /// @param parent The parent file handle.
        /// @param name The name of the file.
        File(Handle parent, std::string_view name);

        /// @param parent The parent file handle.
        /// @param archive The archive this file is in.
        /// @param id The identifier of this file in its archive.
        File(Handle parent, std::shared_ptr<Archive> archive, size_t id);

        /// @param parent The parent file handle.
        /// @param archive The archive mounted on this file.
        /// @param name The name of this file.
        File(Handle parent, std::shared_ptr<Archive> archive, std::string_view name);

        /// Recursively generates an archive's files to the virtual file system.
        /// Called after the archive has been mounted.
        void generateArchive();

        /// Recursively destroys the archive's files from the virtual file system.
        /// Called after the archive is unmounted.
        void destroyArchive();

        /// Adds a child file to this file.
        /// @param child The child file to add.
        void addChild(File::Handle child);

        /// Removes a child file from this file.
        /// @param child The child file to remove.
        void removeChild(File::Handle child);

        /// Finds a child file in this file.
        /// @param name The name of the child file to find.
        /// @return A handle to the child file, or nullptr if the child file does not exist.
        File::Handle findChild(std::string_view name) const;

        /// Recursively destroys a file and its children from the virtual file system.
        /// Called after the file is destroyed.
        void destroyRecursive();

        std::string path; ///< The path of this file.
        std::string name; ///< The name of this file.
        bool directory;   ///< Whether this file is a directory.

        std::shared_ptr<Archive> archive; ///< The archive this file belongs to.
        size_t id;                        ///< The id of this file in its archive.

        Handle parent;  ///< The parent file handle.
        Handle sibling; ///< The next sibling file handle.
        Handle child;   ///< The first child file handle.

        bool destroyed; ///< Whether this file has been marked for deletion.

        std::mutex mutex; ///< The mutex used to synchronize changing properties of this file.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_FILE_HPP
