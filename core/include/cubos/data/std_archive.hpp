#ifndef CUBOS_CORE_DATA_STD_ARCHIVE_HPP
#define CUBOS_CORE_DATA_STD_ARCHIVE_HPP

#include <cubos/data/archive.hpp>

#include <unordered_map>
#include <filesystem>

namespace cubos::core::data
{
    /// Archive implementation which represents a file/directory in the OS file system, implemented with the standard
    /// library.
    class STDArchive : public Archive
    {
    public:
        /// @param osPath The path to the file/directory in the real file system.
        /// @param isDirectory Whether the path is a directory or a file.
        /// @param readOnly True if the archive is read-only, false otherwise.
        STDArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly);
        virtual ~STDArchive() = default;

    protected:
        virtual size_t create(size_t parent, std::string_view name, bool directory = false) override;
        virtual bool destroy(size_t id) override;
        virtual std::string getName(size_t id) const override;
        virtual bool isDirectory(size_t id) const override;
        virtual bool isReadOnly() const override;
        virtual size_t getParent(size_t id) const override;
        virtual size_t getSibling(size_t id) const override;
        virtual size_t getChild(size_t id) const override;
        virtual std::unique_ptr<memory::Stream> open(File::Handle file, File::OpenMode mode) override;

    private:
        /// Information about a file in the directory.
        struct FileInfo
        {
            std::filesystem::path osPath; ///< The path to the file in the real file system.
            size_t parent;                ///< The identifier of the parent file.
            size_t sibling;               ///< The identifier of the next sibling file.
            size_t child;                 ///< The identifier of the first child file.
            bool directory;               ///< True if the file is a directory, false otherwise.
        };

        /// Recursively adds all files in the directory to the archive.
        void generate(size_t parent);

        std::filesystem::path osPath;               ///< The path to the directory in the real file system.
        bool readOnly;                              ///< True if the archive is read-only, false otherwise.
        std::unordered_map<size_t, FileInfo> files; ///< Maps file identifiers to file info.
        size_t nextId;                              ///< The next identifier to assign to a file.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_ARCHIVE_HPP
