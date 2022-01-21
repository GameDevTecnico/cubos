#include <cubos/data/std_archive.hpp>
#include <cubos/data/file_stream.hpp>
#include <cubos/memory/std_stream.hpp>
#include <cubos/log.hpp>

using namespace cubos;
using namespace cubos::data;

STDArchive::STDArchive(const std::filesystem::path& osPath, bool isDirectory, bool readOnly)
    : osPath(osPath), readOnly(readOnly)
{
    // Check if the file/directory exists.
    if (!std::filesystem::exists(osPath))
    {
        if (readOnly)
        {
            logError("Couldn't create STDArchive: file/directory '{}' doesn't exist", osPath.string());
            abort();
        }
        else
        {
            // Create the file/directory.
            if (isDirectory)
                std::filesystem::create_directory(osPath);
            else
            {
                // Write an empty file.
                std::string path = osPath.string();
                FILE* file = fopen(path.c_str(), "w");
                if (!file)
                {
                    logError("Couldn't create STDArchive: couldn't create root file '{}'", path);
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
            logError("Couldn't create STDArchive: expected a file at '{}', found a directory", osPath.string());
            abort();
        }

        // Add all children files to the archive.
        this->files[1] = {osPath, 0, 0, 0, true};
        this->nextId = 2;
        this->generate(1);
    }
    else
    {
        if (isDirectory)
        {
            logError("Couldn't create STDArchive: expected a directory at '{}', found a file", osPath.string());
            abort();
        }

        this->files[1] = {osPath, 0, 0, 0, false};
    }
}

void STDArchive::generate(size_t parent)
{
    auto& parentInfo = this->files[parent];

    // Iterate over all files in the directory.
    for (auto& entry : std::filesystem::directory_iterator(parentInfo.osPath))
    {
        // Get the path to the file.
        auto osPath = entry.path();

        // Add the file to the tree.
        auto directory = std::filesystem::is_directory(osPath);
        size_t id = this->nextId++;
        this->files[id] = {osPath, parent, parentInfo.child, 0, directory};
        parentInfo.child = id;

        // Recursively add all files in the directory.
        if (directory)
            this->generate(id);
    }
}

size_t STDArchive::create(size_t parent, std::string_view name, bool directory)
{
    // Make sure that the archive isn't read-only and the parent is a directory.
    if (this->readOnly || parent == 0 || !this->isDirectory(parent))
        return 0;

    // Create the file/directory in the OS file system.
    auto& parentInfo = this->files.at(parent);
    auto osPath = parentInfo.osPath / name;
    if (directory)
    {
        if (!std::filesystem::create_directory(osPath))
            return false;
    }
    else
    {
        // Write an empty file.
        std::string path = osPath.string();
        FILE* file = fopen(path.c_str(), "w");
        if (!file)
            return false;
        fclose(file);
    }

    // Add the file to the tree.
    size_t id = this->nextId++;
    this->files[id] = {osPath, parent, parentInfo.child, 0, directory};
    parentInfo.child = id;
    return id;
}

bool STDArchive::destroy(size_t id)
{
    // Make sure the file isn't read only, that the file exist and that it's not the root.
    if (this->readOnly || this->files.count(id) == 0 || id == 1)
        return false;

    // Make sure the file isn't a non-empty directory.
    const auto& info = this->files.at(id);
    if (info.directory && info.child != 0)
        return false;

    // Remove the file from the real file system.
    if (!std::filesystem::remove(info.osPath))
        return false;

    // Remove the file from the tree.
    auto& parentInfo = this->files.at(info.parent);
    if (parentInfo.child == id)
        parentInfo.child = info.sibling;
    else
    {
        size_t sibling = parentInfo.child;
        while (sibling != 0)
        {
            auto siblingSibling = this->files.at(sibling).sibling;
            if (siblingSibling == id)
            {
                this->files.at(sibling).sibling = info.sibling;
                break;
            }
            sibling = siblingSibling;
        }
    }
    this->files.erase(id);
    return true;
}

std::string STDArchive::getName(size_t id) const
{
    auto it = this->files.find(id);
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't get name of file, file doesn't exist");
        abort();
    }

    return it->second.osPath.filename().string();
}

bool STDArchive::isDirectory(size_t id) const
{
    auto it = this->files.find(id);
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't check if file is directory, file doesn't exist");
        abort();
    }

    return it->second.directory;
}

bool STDArchive::isReadOnly() const
{
    return this->readOnly;
}

size_t STDArchive::getParent(size_t id) const
{
    auto it = this->files.find(id);
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't get parent of file, file doesn't exist");
        abort();
    }

    return it->second.parent;
}

size_t STDArchive::getSibling(size_t id) const
{
    auto it = this->files.find(id);
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't get sibling of file, file doesn't exist");
        abort();
    }

    return it->second.sibling;
}

size_t STDArchive::getChild(size_t id) const
{
    auto it = this->files.find(id);
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't get child of file, file doesn't exist");
        abort();
    }

    return it->second.child;
}

std::unique_ptr<memory::Stream> STDArchive::open(File::Handle file, File::OpenMode mode)
{
    // Check if the archive is read-only.
    if (this->readOnly && mode != File::OpenMode::Read)
    {
        logError("STDArchive: Couldn't open file for writing, archive is read-only");
        abort();
    }

    // Check if the file exists.
    auto it = this->files.find(file->getId());
    if (it == this->files.end())
    {
        logError("STDArchive: Couldn't open file, file doesn't exist");
        abort();
    }

    // Check if the file is a directory.
    if (it->second.directory)
    {
        logError("STDArchive: Couldn't open file, file is a directory");
        abort();
    }

    // Open the file.
    const char* std_mode = mode == File::OpenMode::Write ? "wb" : "rb";
    std::string path  = it->second.osPath.string();
    return std::make_unique<FileStream<memory::StdStream>>(
        file, mode, std::move(memory::StdStream(fopen(path.c_str(), std_mode), true)));
}
