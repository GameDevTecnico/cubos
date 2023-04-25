#ifndef CUBOS_CORE_DATA_STD_ARCHIVE_HPP
#define CUBOS_CORE_DATA_STD_ARCHIVE_HPP

#include <filesystem>
#include <unordered_map>

#include <cubos/core/data/archive.hpp>

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
        virtual std::size_t create(std::size_t parent, std::string_view name, bool directory = false) override;
        virtual bool destroy(std::size_t id) override;
        virtual std::string getName(std::size_t id) const override;
        virtual bool isDirectory(std::size_t id) const override;
        virtual bool isReadOnly() const override;
        virtual std::size_t getParent(std::size_t id) const override;
        virtual std::size_t getSibling(std::size_t id) const override;
        virtual std::size_t getChild(std::size_t id) const override;
        virtual std::unique_ptr<memory::Stream> open(File::Handle file, File::OpenMode mode) override;

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

        std::filesystem::path osPath;                    ///< The path to the directory in the real file system.
        bool readOnly;                                   ///< True if the archive is read-only, false otherwise.
        std::unordered_map<std::size_t, FileInfo> files; ///< Maps file identifiers to file info.
        std::size_t nextId;                              ///< The next identifier to assign to a file.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_ARCHIVE_HPP
