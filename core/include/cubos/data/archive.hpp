#ifndef CUBOS_CORE_DATA_ARCHIVE_HPP
#define CUBOS_CORE_DATA_ARCHIVE_HPP

#include <cubos/data/file.hpp>

namespace cubos::core::data
{
    /// Represents an archive in the virtual file system.
    /// This class is meant to only be used internally by the virtual file system.
    ///
    /// Each file in an archive is represented by a unique integer identifier.
    /// The identifier 0 is reserved for errors, and the identifier 1 is reserved for the root file/directory.
    class Archive
    {
    public:
        Archive() = default;
        virtual ~Archive() = default;

    protected:
        friend File;

        /// Creates a new file in the archive. If the file already exists, or if the path is relative, 0 is returned.
        /// @param parent The parent directory of the new file.
        /// @param name The name of the new file.
        /// @return The identifier of the file, or 0 if the file could not be created.
        virtual size_t create(size_t parent, std::string_view name, bool directory = false) = 0;

        /// Destroys a file in the archive. If the file doesn't exist, or if it's a non-empty directory, 0 is returned.
        /// @param id The identifier of the file to delete.
        /// @return True if the file was destroyed, false otherwise.
        virtual bool destroy(size_t id) = 0;

        /// Gets the name of a file.
        /// @param id The identifier of the file.
        /// @return The name of the file.
        virtual std::string getName(size_t id) const = 0;

        /// Checks if a file is a directory.
        /// @param id The identifier of the file.
        /// @return True if the file is a directory, false otherwise.
        virtual bool isDirectory(size_t id) const = 0;

        /// Checks if the archive is read-only.
        /// @return True if the archive is read-only, false otherwise.
        virtual bool isReadOnly() const = 0;

        /// Gets the parent of a file.
        /// @param id The identifier of the file.
        /// @return The identifier of the parent file, or 0 if the file is the root file.
        virtual size_t getParent(size_t id) const = 0;

        /// Gets the next sibling of a file.
        /// @param id The identifier of the file.
        /// @return The identifier of the next sibling, or 0 if the file is the last child of its parent.
        virtual size_t getSibling(size_t id) const = 0;

        /// Gets the first child of a directory.
        /// @param id The identifier of the directory.
        /// @return The identifier of the first child, or 0 if the file is not a directory or if it is empty.
        virtual size_t getChild(size_t id) const = 0;

        /// Opens a file in the archive.
        /// If the file does not exist, or the archive is read-only, nullptr is returned.
        /// @param file The handle of the file.
        /// @param mode The mode to open the file in.
        /// @return The file stream, or nullptr if the file could not be opened.
        virtual std::unique_ptr<memory::Stream> open(File::Handle file, File::OpenMode mode) = 0;
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_ARCHIVE_HPP
