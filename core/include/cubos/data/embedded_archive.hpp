#ifndef CUBOS_DATA_EMBEDDED_ARCHIVE_HPP
#define CUBOS_DATA_EMBEDDED_ARCHIVE_HPP

#include <cubos/data/archive.hpp>

#include <vector>
#include <map>

namespace cubos::data
{
    /// Archive implementation used to load data embedded in the executable.
    class EmbeddedArchive : public Archive
    {
    public:
        /// Structure of the data embedded in the executable.
        struct Data
        {
            /// Represents a file entry in the embedded archive.
            struct Entry
            {
                const char* name; ///< The name of the file entry.
                bool isDirectory; ///< Whether the entry is a directory.

                size_t parent;  ///< The index of the parent directory.
                size_t sibling; ///< The index of the next sibling.
                size_t child;   ///< The index of the first child.

                const void* data; ///< The data of the file entry.
                size_t size;      ///< The size of the data.
            };

            const Entry* entries; ///< The entries of the embedded archive.
            size_t entryCount;    ///< The number of entries in the embedded archive.
        };

        /// @param name The name of the data the embedded archive represents.
        EmbeddedArchive(const std::string& name);
        virtual ~EmbeddedArchive() = default;

        /// Registers a embedded archive data.
        /// @param name The name of the embedded archive data.
        /// @param data The embedded archive data.
        static void registerData(const std::string& name, const Data& data);

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
        /// Returns the registry of embedded archive data.
        static std::map<std::string, const Data&>& getRegistry();

        const Data* data; ///< The embedded archive data.
    };
} // namespace cubos::data

#endif // CUBOS_DATA_EMBEDDED_ARCHIVE_HPP
