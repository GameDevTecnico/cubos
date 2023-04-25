#ifndef CUBOS_CORE_DATA_EMBEDDED_ARCHIVE_HPP
#define CUBOS_CORE_DATA_EMBEDDED_ARCHIVE_HPP

#include <map>
#include <vector>

#include <cubos/core/data/archive.hpp>

namespace cubos::core::data
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

                std::size_t parent;  ///< The index of the parent directory.
                std::size_t sibling; ///< The index of the next sibling.
                std::size_t child;   ///< The index of the first child.

                const void* data; ///< The data of the file entry.
                std::size_t size; ///< The size of the data.
            };

            const Entry* entries;   ///< The entries of the embedded archive.
            std::size_t entryCount; ///< The number of entries in the embedded archive.
        };

        /// @param name The name of the data the embedded archive represents.
        EmbeddedArchive(const std::string& name);
        virtual ~EmbeddedArchive() = default;

        /// Registers a embedded archive data.
        /// @param name The name of the embedded archive data.
        /// @param data The embedded archive data.
        static void registerData(const std::string& name, const Data& data);

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
        /// Returns the registry of embedded archive data.
        static std::map<std::string, const Data&>& getRegistry();

        const Data* data; ///< The embedded archive data.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_EMBEDDED_ARCHIVE_HPP
