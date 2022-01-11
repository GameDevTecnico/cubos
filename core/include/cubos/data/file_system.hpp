#ifndef CUBOS_DATA_FILE_SYSTEM_HPP
#define CUBOS_DATA_FILE_SYSTEM_HPP

#include <cubos/data/file.hpp>

namespace cubos::data
{
    /// Represents the virtual file system of the engine.
    /// This class is never instantiated, and is only used to provide static methods.
    ///
    /// Example 1:
    ///     FileSystem::mount("/assets", ...);
    ///     ...
    ///     auto modelsDir = FileSystem::find("/assets/models");
    ///     if (modelsDir != nullptr && modelsDir->isDirectory())
    ///         for (auto model = modelsDir->getChild(); model != nullptr; model = model->getSibling())
    ///             loadModel(*model->open(File::OpenMode::Read));
    ///     ...
    ///     FileSystem::unmount("/assets");
    ///
    /// Example 2:
    ///     FileSystem::mount("/settings.yaml", ...);
    ///     ...
    ///     auto settingsFile = FileSystem::root()->find("settings.yaml");
    ///     if (settingsFile)
    ///         loadSettings(*model->open(File::OpenMode::Read));
    ///     ...
    class FileSystem final
    {
    public:
        FileSystem() = delete;
        ~FileSystem() = delete;

        /// Gets a handle to the root file.
        static File::Handle root();

        /// Mounts an archive to a path in the virtual file system of the engine.
        /// The directory which contains the path must exist, but the path itself must not (unless it is the root).
        /// Archives can be mounted to the root path ("/") or to any other path.
        ///
        /// If the path isn't absolute, or if the parent directory of the path doesn't exist, or if there's already an
        /// archive mounted at the path, abort() is called.
        /// @param path The path to mount the archive to.
        /// @param archive The archive to mount.
        static void mount(std::string_view path, std::shared_ptr<Archive> archive);

        /// Unmounts an archive from a path in the virtual file system of the engine.
        /// If no archive is mounted at the given path, nothing is done.
        /// @param path The path to unmount the archive from.
        static void unmount(std::string_view path);

        /// Finds a file in the virtual file system, given its absolute path.
        /// @param path The absolute path to the file.
        /// @return A handle to the file, or nullptr if the file does not exist or the path is relative.
        static File::Handle find(std::string_view path);

        /// Creates a new file in the virtual file system.
        /// @param path The absolute path to the file.
        /// @param directory True if the file should be a directory, false otherwise.
        /// @return A handle to the file, or nullptr if the file could not be created or the path is relative.
        static File::Handle create(std::string_view path, bool directory = false);

        /// Destroys a file in the virtual file system. If the file is a directory, all children of the directory are
        /// destroyed as well.
        /// @param path The absolute path of the file to destroy.
        /// @return True if the file was marked for destruction, false otherwise.
        static bool destroy(std::string_view path);

        /// Opens a file in the virtual file system.
        /// @param path The absolute path to the file.
        /// @param mode The mode to open the file in.
        /// @return A handle to the file stream, or nullptr if an error occurred.
        static std::unique_ptr<memory::Stream> open(std::string_view path, File::OpenMode mode);
    };
} // namespace cubos::data

#endif // CUBOS_DATA_FILE_SYSTEM_HPP
