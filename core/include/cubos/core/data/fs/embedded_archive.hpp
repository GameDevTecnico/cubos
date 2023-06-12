/// @file
/// @brief Defines the EmbeddedArchive class, which implements the Archive interface class.

#pragma once

#include <map>
#include <vector>

#include <cubos/core/data/fs/archive.hpp>

namespace cubos::core::data
{
    /// @brief Archive implementation which allows reading from data embedded in the application as
    /// if it was a regular file or directory. Meant to be used with the `cubinhos embed` tool.
    ///
    /// @details This archive works by provide a global map of data, where embedded data can be
    /// registered with a given name. Then, to access that data, an instance of this archive should
    /// be created with the same name, and then mounted anywhere on the file system.
    ///
    /// The embed tool works by by generating source files which define and register that data. By
    /// linking those files into your application, your making the data available to be accessed
    /// through an instance of this archive.
    ///
    /// @see Archive
    class EmbeddedArchive : public Archive
    {
    public:
        /// @brief Describes the structure of the embedded data.
        struct Data
        {
            /// @brief Describes a file entry in the embedded data.
            struct Entry
            {
                const char* name; ///< Name of the file entry.
                bool isDirectory; ///< Whether the entry is a directory.

                std::size_t parent;  ///< Index of the parent directory.
                std::size_t sibling; ///< Index of the next sibling.
                std::size_t child;   ///< Index of the first child.

                const void* data; ///< Data of the file entry.
                std::size_t size; ///< Size of the data.
            };

            const Entry* entries;   ///< Entries of the embedded archive.
            std::size_t entryCount; ///< Number of entries in the embedded archive.
        };

        /// @param name Name of the data the embedded archive represents. Must match a registered
        /// data instance - aborts otherwise.
        EmbeddedArchive(const std::string& name);
        ~EmbeddedArchive() override = default;

        /// Registers a embedded archive data.
        /// @param name The name of the embedded archive data.
        /// @param data The embedded archive data.
        static void registerData(const std::string& name, const Data& data);

        // Archive interface implementation.

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
        /// @returns Registry of embedded archive data.
        static std::map<std::string, const Data&>& registry();

        const Data* mData; ///< The embedded archive data.
    };
} // namespace cubos::core::data
