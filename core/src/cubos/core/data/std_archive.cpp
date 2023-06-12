#include <cubos/core/data/file_stream.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/std_stream.hpp>

using cubos::core::data::STDArchive;
using cubos::core::memory::Stream;

#define INIT_OR_RETURN(ret)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (mFiles.empty())                                                                                            \
        {                                                                                                              \
            CUBOS_ERROR("Archive was not initialized successfully");                                                   \
            return (ret);                                                                                              \
        }                                                                                                              \
    } while (false)

STDArchive::STDArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly)
    : mOsPath(osPath)
    , mReadOnly(readOnly)
{
    // Check if the file/directory exists.
    if (!std::filesystem::exists(osPath))
    {
        if (readOnly)
        {
            CUBOS_ERROR("File/directory '{}' does not exist on the host file system", osPath.string());
            return;
        }
        else
        {
            // Create the file/directory.
            if (isDirectory)
            {
                std::error_code err;
                if (!std::filesystem::create_directory(osPath, err))
                {
                    CUBOS_ERROR("std::filesystem::create_directory() failed: {}", err.message());
                    CUBOS_ERROR("Could not create root directory '{}' on the host file system", osPath.string());
                    return;
                }
            }
            else
            {
                // Write an empty file.
                std::string path = osPath.string();
                FILE* file = fopen(path.c_str(), "w");
                if (file == nullptr)
                {
                    CUBOS_ERROR("fopen() failed: {}", strerror(errno));
                    CUBOS_ERROR("Could not create root file '{}' on the host file system", path);
                    return;
                }
                fclose(file);
            }
        }
    }

    // Check if it's a directory.
    if (std::filesystem::is_directory(osPath))
    {
        if (!isDirectory)
        {
            CUBOS_ERROR("Expected regular file at '{}' on the host file system, found a directory", osPath.string());
            return;
        }

        // Add all children files to the archive.
        mFiles[1] = {osPath, 0, 0, 0, true};
        mNextId = 2;
        this->generate(1);
    }
    else
    {
        if (isDirectory)
        {
            CUBOS_ERROR("Expected directory at '{}' on the host file system, found a regular file", osPath.string());
            return;
        }

        mFiles[1] = {osPath, 0, 0, 0, false};
    }
}

void STDArchive::generate(std::size_t parent)
{
    auto& parentInfo = mFiles[parent];

    // Iterate over all files in the directory.
    for (const auto& entry : std::filesystem::directory_iterator(parentInfo.osPath))
    {
        // Get the path to the file.
        const auto& osPath = entry.path();

        // Add the file to the tree.
        auto directory = std::filesystem::is_directory(osPath);
        std::size_t id = mNextId++;
        mFiles[id] = {osPath, parent, parentInfo.child, 0, directory};
        parentInfo.child = id;

        // Recursively add all files in the directory.
        if (directory)
        {
            this->generate(id);
        }
    }
}

std::size_t STDArchive::create(std::size_t parent, std::string_view name, bool directory)
{
    INIT_OR_RETURN(0);
    CUBOS_DEBUG_ASSERT(!mReadOnly);
    CUBOS_DEBUG_ASSERT(this->directory(parent));

    // Create the file/directory in the OS file system.
    auto& parentInfo = mFiles.at(parent);
    auto osPath = parentInfo.osPath / name;
    if (directory)
    {
        std::error_code err;
        if (!std::filesystem::create_directory(osPath, err))
        {
            CUBOS_ERROR("std::filesystem::create_directory() failed: {}", err.message());
            return 0U;
        }
    }
    else
    {
        // Write an empty file.
        std::string path = osPath.string();
        FILE* file = fopen(path.c_str(), "w");
        if (file == nullptr)
        {
            CUBOS_ERROR("fopen() failed: {}", strerror(errno));
            return 0U;
        }
        fclose(file);
    }

    // Add the file to the tree.
    std::size_t id = mNextId++;
    mFiles[id] = {osPath, parent, parentInfo.child, 0, directory};
    parentInfo.child = id;
    return id;
}

bool STDArchive::destroy(std::size_t id)
{
    INIT_OR_RETURN(false);
    CUBOS_DEBUG_ASSERT(!mReadOnly);
    CUBOS_DEBUG_ASSERT(mFiles.contains(id));
    CUBOS_DEBUG_ASSERT(id != 1);

    // Make sure the file isn't a non-empty directory.
    const auto& info = mFiles.at(id);
    CUBOS_DEBUG_ASSERT(!info.directory || info.child == 0);

    // Remove the file from the real file system.
    std::error_code err;
    if (!std::filesystem::remove(info.osPath, err))
    {
        CUBOS_ERROR("std::filesystem::remove() failed: {}", err.message());
        return false;
    }

    // Remove the file from the tree.
    auto& parentInfo = mFiles.at(info.parent);
    if (parentInfo.child == id)
    {
        parentInfo.child = info.sibling;
    }
    else
    {
        std::size_t sibling = parentInfo.child;
        while (sibling != 0)
        {
            auto siblingSibling = mFiles.at(sibling).sibling;
            if (siblingSibling == id)
            {
                mFiles.at(sibling).sibling = info.sibling;
                break;
            }
            sibling = siblingSibling;
        }
    }
    mFiles.erase(id);
    return true;
}

std::string STDArchive::name(std::size_t id) const
{
    INIT_OR_RETURN("<invalid>");

    auto it = mFiles.find(id);
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    return it->second.osPath.filename().string();
}

bool STDArchive::directory(std::size_t id) const
{
    INIT_OR_RETURN(false);

    auto it = mFiles.find(id);
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    return it->second.directory;
}

bool STDArchive::readOnly() const
{
    return mReadOnly;
}

std::size_t STDArchive::parent(std::size_t id) const
{
    INIT_OR_RETURN(0);

    auto it = mFiles.find(id);
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    return it->second.parent;
}

std::size_t STDArchive::sibling(std::size_t id) const
{
    INIT_OR_RETURN(0);

    auto it = mFiles.find(id);
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    return it->second.sibling;
}

std::size_t STDArchive::child(std::size_t id) const
{
    INIT_OR_RETURN(0);

    auto it = mFiles.find(id);
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    return it->second.child;
}

std::unique_ptr<Stream> STDArchive::open(File::Handle file, File::OpenMode mode)
{
    INIT_OR_RETURN(nullptr);
    CUBOS_DEBUG_ASSERT(file->archive().get() == this);
    CUBOS_DEBUG_ASSERT(!mReadOnly || mode == File::OpenMode::Read);

    auto it = mFiles.find(file->id());
    CUBOS_DEBUG_ASSERT(it != mFiles.end());
    CUBOS_DEBUG_ASSERT(!it->second.directory);

    const char* stdMode = mode == File::OpenMode::Write ? "wb" : "rb";
    std::string path = it->second.osPath.string();
    auto fd = fopen(path.c_str(), stdMode);
    if (fd == nullptr)
    {
        CUBOS_ERROR("fopen() failed: {}", strerror(errno));
        return nullptr;
    }

    return std::make_unique<FileStream<memory::StdStream>>(file, mode, memory::StdStream(fd, true));
}
