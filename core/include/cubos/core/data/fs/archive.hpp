/// @file
/// @brief Class @ref cubos::core::data::Archive.
/// @ingroup core-data-fs

#pragma once

#include <cubos/core/data/fs/file.hpp>

namespace cubos::core::data
{
    /// @brief Interface for a bridge between the CUBOS. virtual file system and the real world.
    ///
    /// @note Should never be used directly by the user - instead, use the FileSystem and File
    /// classes.
    ///
    /// By real world, we mean something such as a file on the OS file system or a network
    /// connection. The archive is responsible for creating and destroying files, supplying streams
    /// for reading and writing, and retrieving information about existing files.
    ///
    /// Each file in an archive is represented by a unique integer identifier.
    /// The identifier 0 is reserved for errors, and the identifier 1 is reserved for the root file
    /// of the archive.
    ///
    /// The root file may be a directory or a regular file. If its a directory, when mounted, the
    /// archive will take the form of a directory, with the root file being placed on the mount
    /// point. If the root file is a regular file, then the mount point will be a regular file.
    ///
    /// This class is not meant to be thread-safe - it is the responsibility of the @ref File class
    /// to ensure that the same file is not accessed or modified by multiple threads at the same
    /// time.
    ///
    /// Implementations should crash when called with invalid arguments - since they're only called
    /// by @ref File, its okay to assume the arguments are correct (please do use asserts though).
    ///
    /// @see File
    /// @see FileSystem
    /// @ingroup core-data-fs
    class CUBOS_CORE_API Archive
    {
    public:
        Archive() = default;
        virtual ~Archive() = default;

        /// @brief Creates a new file in the archive.
        ///
        /// The first child of the @p parent will have its sibling set to the new file and then be
        /// replaced by the new file.
        ///
        /// @param parent Parent directory of the new file.
        /// @param name Name of the new file.
        /// @param directory Whether the new file is a directory or not.
        /// @return Identifier of the file, or 0 if the file could not be created.
        virtual std::size_t create(std::size_t parent, std::string_view name, bool directory = false) = 0;

        /// @brief Destroys a regular file or empty directory in the archive.
        ///
        /// Will be replaced in the tree by its sibling.
        ///
        /// @param id Identifier of the file.
        /// @return Whether the file was destroyed successfully.
        virtual bool destroy(std::size_t id) = 0;

        /// @brief Gets the name of the file with the given @p id.
        /// @param id Identifier of the file.
        /// @return Name of the file.
        virtual std::string name(std::size_t id) const = 0;

        /// @brief Checks whether the file with the given @p id is a directory.
        /// @param id Identifier of the file.
        /// @return Whether the file is a directory or not.
        virtual bool directory(std::size_t id) const = 0;

        /// @brief Checks whether the archive is read-only.
        /// @return Whether the archive is read-only.
        virtual bool readOnly() const = 0;

        /// @brief Gets the identifier of the parent file of the file with the given @p id.
        /// @param id Identifier of the file.
        /// @return Identifier of the parent file, or 0 if the file is the root file.
        virtual std::size_t parent(std::size_t id) const = 0;

        /// @brief Gets the identifier of the sibling of the file with the given @p id.
        /// @param id Identifier of the file.
        /// @return Identifier of the sibling, or 0 if there are no more files in the parent.
        virtual std::size_t sibling(std::size_t id) const = 0;

        /// @brief Gets the identifier of the first child of the file with the given @p id.
        /// @param id Identifier of the file.
        /// @return Identifier of the first child, or 0 if the file is not a directory or is empty.
        virtual std::size_t child(std::size_t id) const = 0;

        /// @brief Opens a file in the archive.
        ///
        /// Although a bit hacky, the @p handle parameter is used to keep a reference to the
        /// respective @p File alive, preventing the file from being destroyed while the stream is
        /// open.
        ///
        /// @param id Identifier of the file.
        /// @param handle Handle to the file.
        /// @param mode Mode to open the file in.
        /// @return File stream, or nullptr if the file could not be opened.
        virtual std::unique_ptr<memory::Stream> open(std::size_t id, File::Handle handle, File::OpenMode mode) = 0;
    };
} // namespace cubos::core::data
