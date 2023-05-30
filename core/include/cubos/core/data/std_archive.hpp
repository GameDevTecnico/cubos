/// @file
/// @brief Defines the STDArchive class, which implements the Archive interface class.

#pragma once

#include <filesystem>
#include <unordered_map>

#include <cubos/core/data/archive.hpp>

namespace cubos::core::data
{
    /// @brief Archive implementation which allows reading and writing into a file in the OS file
    /// system, using the standard library.
    ///
    /// @details Can interface with both regular and directory files. If the archive is not
    /// read-only and the target file is not found on the OS file system, then it is created.
    ///
    /// This implementation does not detect changes in the file system made without the
    /// File and FileSystem classes (see #263).
    ///
    /// @see Archive
    class STDArchive : public Archive
    {
    public:
        /// @param osPath The path to the file/directory in the real file system.
        /// @param isDirectory Whether the path is a directory or a file.
        /// @param readOnly True if the archive is read-only, false otherwise.
        STDArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly);
        ~STDArchive() override = default;

        std::size_t create(std::size_t parent, std::string_view name, bool directory = false) override;
        bool destroy(std::size_t id) override;
        std::string name(std::size_t id) const override;
        bool directory(std::size_t id) const override;
        bool readOnly() const override;
        std::size_t parent(std::size_t id) const override;
        std::size_t sibling(std::size_t id) const override;
        std::size_t child(std::size_t id) const override;
        std::unique_ptr<memory::Stream> open(std::size_t id, File::Handle handle, File::OpenMode mode) override;

    private:
        /// Information about a file in the directory.
        struct FileInfo
        {
            std::filesystem::path osPath; ///< The path to the file in the real file system.
            std::size_t parent;           ///< The identifier of the parent file.
            std::size_t sibling;          ///< The identifier of the next sibling file.
            std::size_t child;            ///< The identifier of the first child file.
            bool directory;               ///< True if the file is a directory, false otherwise.
        };

        /// Recursively adds all files in the directory to the archive.
        void generate(std::size_t parent);

        std::filesystem::path mOsPath;                    ///< The path to the directory in the real file system.
        bool mReadOnly;                                   ///< True if the archive is read-only, false otherwise.
        std::unordered_map<std::size_t, FileInfo> mFiles; ///< Maps file identifiers to file info.
        std::size_t mNextId;                              ///< The next identifier to assign to a file.
    };
} // namespace cubos::core::data
