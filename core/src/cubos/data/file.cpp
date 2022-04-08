#include <cubos/data/file.hpp>
#include <cubos/data/archive.hpp>
#include <cubos/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

File::File(Handle parent, std::string_view name)
{
    this->name = name;
    this->directory = true; // Files outside of archives are always directories.
    this->archive = nullptr;
    this->id = 0;
    this->parent = parent;
    this->destroyed = false;
    if (this->parent)
        this->path = this->parent->path + "/" + std::string(this->name);
}

File::File(Handle parent, std::shared_ptr<Archive> archive, size_t id)
{
    this->name = archive->getName(id);
    this->directory = archive->isDirectory(id);
    this->archive = archive;
    this->id = id;
    this->parent = parent;
    this->destroyed = false;
    if (this->parent)
        this->path = this->parent->path + "/" + std::string(this->name);
}

File::File(Handle parent, std::shared_ptr<Archive> archive, std::string_view name)
{
    this->name = name;
    this->directory = archive->isDirectory(1);
    this->archive = archive;
    this->id = 1;
    this->parent = parent;
    this->destroyed = false;
    if (this->parent)
        this->path = this->parent->path + "/" + std::string(this->name);
}

void File::mount(std::string_view path, std::shared_ptr<Archive> archive)
{
    // Remove trailing slashes.
    while (path.ends_with('/'))
        path.remove_suffix(1);

    // Split path and find the parent directory.
    auto i = path.find_last_of('/');
    auto dir = this->find(i == std::string::npos ? "" : path.substr(0, i));
    auto name = i == std::string::npos ? path : path.substr(i);

    // Check if the directory exists.
    if (!dir)
    {
        logError("Could not mount archive at path '{}' relative to '{}', the parent directory '{}' does not exist",
                 path, this->path, path.substr(0, i));
        abort();
    }

    // Lock mutex of the directory.
    std::lock_guard<std::mutex> dir_lock(dir->mutex);

    // Mount the archive in the directory itself (must be the root directory).
    if (name.empty())
    {
        if (dir->parent != nullptr)
        {
            logError("Could not mount archive at path '{}', a file already exists at that path", dir->path);
            abort();
        }

        if (dir->archive != nullptr)
        {
            logError("Could not mount archive at root, an archive has already been mounted at root");
            abort();
        }

        if (!archive->isDirectory(1))
        {
            logError("Could not mount archive at root, archive must be a directory to be mounted at root");
            abort();
        }

        dir->archive = archive;
        dir->id = 1;
        dir->generateArchive();
    }
    // Mount the archive as a child of 'dir'.
    else
    {
        if (dir->findChild(name) != nullptr)
        {
            logError("Could not mount archive at path '{}', a file already exists at that path",
                     dir->path + "/" + std::string(name));
            abort();
        }

        // Add mount point to the directory.
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), archive, name));
        file->generateArchive();
        file->sibling = dir->child;
        dir->child = file;
    }
}

void File::generateArchive()
{
    if (this->archive == nullptr || !this->directory)
        return;

    auto child = this->archive->getChild(this->id);
    while (child != 0)
    {
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), this->archive, child));
        file->sibling = this->child;
        this->child = file;
        file->generateArchive();
        child = this->archive->getSibling(child);
    }
}

void File::unmount(std::string_view path)
{
    // Find mount point.
    auto mountPoint = this->find(path);
    if (mountPoint == nullptr)
    {
        logError("Could not unmount archive at path '{}', no mount point exists at that path", path);
        return;
    }

    // Lock the mount point mutex.
    std::lock_guard<std::mutex> lock(mountPoint->mutex);

    // Check if the file is a mount point.
    if (mountPoint->archive == nullptr)
    {
        logError("Could not unmount archive at path '{}', no archive mounted on that path");
        return;
    }

    // Check if the mount point is the root directory of an archive.
    if (mountPoint->parent != nullptr && mountPoint->archive == mountPoint->parent->archive)
    {
        logError("Could not unmount archive at path '{}', the path must be the mount point (root) of an archive", path);
        return;
    }

    // Recursively unmount all children.
    while (mountPoint->child)
    {
        mountPoint->child->destroyArchive();
        mountPoint->removeChild(mountPoint->child);
    }

    // Remove the mount point its parent directory.
    if (mountPoint->parent != nullptr)
    {
        // Lock the parent directory mutex.
        std::lock_guard<std::mutex> dir_lock(mountPoint->parent->mutex);
        mountPoint->parent->removeChild(mountPoint);
        mountPoint->parent = nullptr;
    }

    mountPoint->archive = nullptr;
    mountPoint->id = 0;
}

void File::destroyArchive()
{
    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(this->mutex);

    if (this->archive != this->parent->archive)
    {
        logError("Could not unmount archive, a file within the archive is mounted on a different archive, which "
                 "must be unmounted first");
        abort();
    }

    while (this->child)
    {
        this->child->destroyArchive();
        this->removeChild(this->child);
    }

    this->parent = nullptr;
    this->archive = nullptr;
    this->id = 0;
}

File::Handle File::find(std::string_view path)
{
    if (path.starts_with("./"))
        path.remove_prefix(2);

    // If the path is empty, return this file.
    if (path.empty())
        return this->shared_from_this();

    // If the path is absolute, or if this file isn't a directory, return nullptr.
    else if (!path.empty() && path[0] == '/' || !this->directory)
        return nullptr;

    // Get name of the first component in the path.
    size_t i = 0;
    for (; i < path.size(); i++)
        if (path[i] == '/')
            break;
    auto name = path.substr(0, i);

    // Remove extra slashes.
    for (; i < path.size(); i++)
        if (path[i] != '/')
            break;

    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(this->mutex);

    // Search for the first child with the given name.
    auto child = this->findChild(name);
    if (child)
        return child->find(path.substr(i));
    else
        return nullptr;
}

File::Handle File::create(std::string_view path, bool directory)
{
    // Remove trailing slashes.
    while (path.ends_with('/'))
        path.remove_suffix(1);

    // Split path and find the parent directory.
    auto i = path.find_last_of('/');
    auto dir = this->find(i == std::string::npos ? "" : path.substr(0, i));
    auto name = i == std::string::npos ? path : path.substr(i);

    // Check if the directory exists.
    if (dir == nullptr)
    {
        logWarning("Could not create file at path '{}' relative to '{}', the parent directory '{}' does not exist",
                   path, this->path, path.substr(0, i));
        return nullptr;
    }

    // Lock the directory mutex.
    std::lock_guard<std::mutex> dir_lock(dir->mutex);

    // Check if the file already exists.
    if (dir->findChild(name) != nullptr)
    {
        logWarning("Could not create file at path '{}', a file already exists at that path",
                   dir->path + "/" + std::string(name));
        return nullptr;
    }

    // Check if the directory is mounted on an archive.
    if (dir->archive == nullptr)
    {
        logWarning("Could not create file at path '{}', parent directory must be mounted on an archive",
                   dir->path + "/" + std::string(name));
        return nullptr;
    }

    // Check if the archive is read-only.
    if (dir->archive->isReadOnly())
    {
        logWarning("Could not create file at path '{}', parent directory is mounted on a read-only archive",
                   dir->path + "/" + std::string(name));
        return nullptr;
    }

    // Create the file.
    size_t id = dir->archive->create(dir->id, name, directory);
    if (id == 0)
    {
        logWarning("Could not create file at path '{}', internal archive error", dir->path + "/" + std::string(name));
        return nullptr;
    }

    // Add the file to the parent directory.
    auto file = std::shared_ptr<File>(new File(dir, dir->archive, id));
    this->addChild(file);
    return file;
}

bool File::destroy()
{
    // Lock the file mutex.
    std::lock_guard file_lock(this->mutex);

    if (this->destroyed)
        return true;
    this->destroyed = true;

    if (this->archive == nullptr)
    {
        logWarning("Could not destroy file '{}', it is not mounted", this->path);
        return false;
    }
    else if (this->archive->isReadOnly())
    {
        logWarning("Could not destroy file '{}', the archive it is mounted on is read-only", this->path);
        return false;
    }
    else if (this->id == 1)
    {
        logWarning("Could not destroy file '{}', the file is the mount point of an archive", this->path);
        return false;
    }

    // Lock the directory mutex and recursively destroy all children.
    while (this->child)
    {
        this->child->destroyRecursive();
        this->removeChild(this->child);
    }

    // Remove the file from the parent directory.
    std::lock_guard dir_lock(this->parent->mutex);
    this->parent->removeChild(this->shared_from_this());
    this->parent = nullptr;

    return true;
}

void File::destroyRecursive()
{
    // Lock the file mutex.
    std::lock_guard file_lock(this->mutex);

    if (this->destroyed)
        return;

    // Mark the file as destroyed.
    this->destroyed = true;
    this->parent = nullptr;

    // Recursively destroy all children.
    while (this->child)
    {
        this->child->destroyRecursive();
        this->removeChild(this->child);
    }
}

std::unique_ptr<memory::Stream> File::open(OpenMode mode)
{
    // Lock the file mutex.
    std::lock_guard file_lock(this->mutex);

    if (this->archive == nullptr)
    {
        logWarning("Could not open file '{}', it is not mounted", this->path);
        return nullptr;
    }
    else if (this->archive->isReadOnly() && mode == OpenMode::Write)
    {
        logWarning("Could not open file '{}' for writing, the archive it is mounted on is read-only", this->path);
        return nullptr;
    }
    else if (this->directory)
    {
        logWarning("Could not open file '{}', the file is a directory", this->path);
        return nullptr;
    }

    // Open the file.
    return this->archive->open(this->shared_from_this(), mode);
}

std::string_view File::getName() const
{
    return this->name;
}

std::string_view File::getPath() const
{
    return this->path;
}

bool File::isDirectory() const
{
    return this->directory;
}

std::shared_ptr<Archive> File::getArchive() const
{
    return this->archive;
}

size_t File::getId() const
{
    return this->id;
}

File::Handle File::getParent() const
{
    return this->parent;
}

File::Handle File::getSibling() const
{
    return this->sibling;
}

File::Handle File::getChild() const
{
    return this->child;
}

void File::addChild(File::Handle child)
{
    child->sibling = this->child;
    this->child = child;
}

void File::removeChild(File::Handle child)
{
    if (this->child == child)
        this->child = child->sibling;
    else
    {
        auto prev = this->child;
        while (prev->sibling != child)
            prev = prev->sibling;
        prev->sibling = child->sibling;
    }
}

File::Handle File::findChild(std::string_view name) const
{
    for (auto child = this->child; child != nullptr; child = child->sibling)
        if (child->name == name)
            return child;
    return nullptr;
}

File::~File()
{
    if (this->destroyed && this->archive)
        if (!this->archive->destroy(this->id))
        {
            logError("Could not destroy file '{}', internal archive error", this->path);
            abort();
        }
}
