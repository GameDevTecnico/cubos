/// @file
/// @brief Class @ref cubos::core::data::StandardArchive.
/// @ingroup core-data-fs

#pragma once

#include <filesystem>
#include <unordered_map>

#include <cubos/core/data/fs/archive.hpp>

namespace cubos::core::data
{
    /// @brief Archive implementation which reads and writes from/into the OS file system using
    /// the standard library.
    ///
    /// Can represent both regular files and directories.
    ///
    /// @todo This implementation does not detect changes in the file system made outside the File
    /// and FileSystem classes (#263).
    ///
    /// @ingroup core-data-fs
    class CUBOS_CORE_API StandardArchive : public Archive
    {
    public:
        ~StandardArchive() override = default;

        /// @brief Constructs pointing to the regular file or directory with the given @p osPath.
        ///
        /// If @p readOnly is false and there's no file or directory at @p osPath, then it is
        /// created.
        ///
        /// @param osPath The path to the file/directory in the real file system.
        /// @param isDirectory Whether the path is a directory or a file.
        /// @param readOnly True if the archive is read-only, false otherwise.
        StandardArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly);

        std::size_t create(std::size_t parent, std::string_view name, bool directory = false) override;
        bool destroy(std::size_t id) override;
        std::string name(std::size_t id) const override;
        bool directory(std::size_t id) const override;
        bool readOnly() const override;
        std::size_t parent(std::size_t id) const override;
        std::size_t sibling(std::size_t id) const override;
        std::size_t child(std::size_t id) const override;
        std::unique_ptr<memory::Stream> open(std::size_t id, File::Handle file, File::OpenMode mode) override;

    private:
        /// @brief Information about a file in the directory.
        struct FileInfo
        {
            std::filesystem::path osPath; ///< Path to the file in the real file system.
            std::size_t parent;           ///< Identifier of the parent file.
            std::size_t sibling;          ///< Identifier of the next sibling file.
            std::size_t child;            ///< Identifier of the first child file.
            bool directory;               ///< True if the file is a directory, false otherwise.
        };

        /// @brief Recursively adds all files in the directory to the archive.
        /// @param parent Id of the directory.
        void generate(std::size_t parent);

        std::filesystem::path mOsPath;                    ///< Path to the directory in the real file system.
        bool mReadOnly;                                   ///< True if the archive is read-only, false otherwise.
        std::unordered_map<std::size_t, FileInfo> mFiles; ///< Maps file identifiers to file info.
        std::size_t mNextId;                              ///< Next identifier to assign to a file.
    };
} // namespace cubos::core::data
