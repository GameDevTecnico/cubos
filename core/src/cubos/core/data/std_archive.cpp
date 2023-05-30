#include <cubos/core/data/file_stream.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/std_stream.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

STDArchive::STDArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly)
    : mOsPath(osPath)
    , mReadOnly(readOnly)
{
    // Check if the file/directory exists.
    if (!std::filesystem::exists(osPath))
    {
        if (readOnly)
        {
            CUBOS_CRITICAL("File/directory '{}' does not exist", osPath.string());
            abort();
        }
        else
        {
            // Create the file/directory.
            if (isDirectory)
            {
                std::filesystem::create_directory(osPath);
            }
            else
            {
                // Write an empty file.
                std::string path = osPath.string();
                FILE* file = fopen(path.c_str(), "w");
                if (file == nullptr)
                {
                    CUBOS_CRITICAL("Could not create root file '{}'", path);
                    abort();
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
            CUBOS_CRITICAL("Expected file at '{}', found directory", osPath.string());
            abort();
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
            CUBOS_CRITICAL("Expected directory at '{}', found file", osPath.string());
            abort();
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
    // Make sure that the archive isn't read-only and the parent is a directory.
    if (mReadOnly || parent == 0 || !this->isDirectory(parent))
    {
        return 0;
    }

    // Create the file/directory in the OS file system.
    auto& parentInfo = mFiles.at(parent);
    auto osPath = parentInfo.osPath / name;
    if (directory)
    {
        if (!std::filesystem::create_directory(osPath))
        {
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
    // Make sure the file isn't read only, that the file exist and that it's not the root.
    if (mReadOnly || !mFiles.contains(id) || id == 1)
    {
        return false;
    }

    // Make sure the file isn't a non-empty directory.
    const auto& info = mFiles.at(id);
    if (info.directory && info.child != 0)
    {
        return false;
    }

    // Remove the file from the real file system.
    if (!std::filesystem::remove(info.osPath))
    {
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

std::string STDArchive::getName(std::size_t id) const
{
    auto it = mFiles.find(id);
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    return it->second.osPath.filename().string();
}

bool STDArchive::isDirectory(std::size_t id) const
{
    auto it = mFiles.find(id);
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    return it->second.directory;
}

bool STDArchive::isReadOnly() const
{
    return mReadOnly;
}

std::size_t STDArchive::getParent(std::size_t id) const
{
    auto it = mFiles.find(id);
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    return it->second.parent;
}

std::size_t STDArchive::getSibling(std::size_t id) const
{
    auto it = mFiles.find(id);
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    return it->second.sibling;
}

std::size_t STDArchive::getChild(std::size_t id) const
{
    auto it = mFiles.find(id);
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    return it->second.child;
}

std::unique_ptr<memory::Stream> STDArchive::open(File::Handle file, File::OpenMode mode)
{
    // Check if the archive is read-only.
    if (mReadOnly && mode != File::OpenMode::Read)
    {
        CUBOS_CRITICAL("Could not open file for writing: archive is read-only");
        abort();
    }

    // Check if the file exists.
    auto it = mFiles.find(file->id());
    if (it == mFiles.end())
    {
        CUBOS_CRITICAL("File does not exist");
        abort();
    }

    // Check if the file is a directory.
    if (it->second.directory)
    {
        CUBOS_CRITICAL("Could not open file: is directory");
        abort();
    }

    // Open the file.
    const char* stdMode = mode == File::OpenMode::Write ? "wb" : "rb";
    std::string path = it->second.osPath.string();
    return std::make_unique<FileStream<memory::StdStream>>(file, mode,
                                                           memory::StdStream(fopen(path.c_str(), stdMode), true));
}
