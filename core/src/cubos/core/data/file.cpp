#include <utility>

#include <cubos/core/data/archive.hpp>
#include <cubos/core/data/file.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

File::File(Handle parent, std::string_view name)
{
    mName = name;
    mDirectory = true; // Files outside of archives are always directories.
    mArchive = nullptr;
    mId = 0;
    mParent = std::move(parent);
    mDestroyed = false;
    if (mParent)
    {
        mPath = mParent->mPath + "/" + std::string(mName);
    }
}

File::File(Handle parent, const std::shared_ptr<Archive>& archive, std::size_t id)
{
    mName = archive->getName(id);
    mDirectory = archive->isDirectory(id);
    mArchive = archive;
    mId = id;
    mParent = std::move(parent);
    mDestroyed = false;
    if (mParent)
    {
        mPath = mParent->mPath + "/" + std::string(mName);
    }
}

File::File(Handle parent, const std::shared_ptr<Archive>& archive, std::string_view name)
{
    mName = name;
    mDirectory = archive->isDirectory(1);
    mArchive = archive;
    mId = 1;
    mParent = std::move(parent);
    mDestroyed = false;
    if (mParent)
    {
        mPath = mParent->mPath + "/" + std::string(mName);
    }
}

void File::mount(std::string_view path, const std::shared_ptr<Archive>& archive)
{
    // Remove trailing slashes.
    while (path.ends_with('/'))
    {
        path.remove_suffix(1);
    }

    // Split path and find the parent directory.
    auto i = path.find_last_of('/');
    auto dir = this->find(i == std::string::npos ? "" : path.substr(0, i));
    auto name = i == std::string::npos ? path : path.substr(i);

    // Check if the directory exists.
    if (!dir)
    {
        CUBOS_CRITICAL(
            "Could not mount archive at path '{}' relative to '{}', the parent directory '{}' does not exist", path,
            mPath, path.substr(0, i));
        abort();
    }

    // Lock mutex of the directory.
    std::lock_guard<std::mutex> dirLock(dir->mMutex);

    // Mount the archive in the directory itself (must be the root directory).
    if (name.empty())
    {
        if (dir->mParent != nullptr)
        {
            CUBOS_CRITICAL("Could not mount archive at path '{}', a file already exists at that path", dir->mPath);
            abort();
        }

        if (dir->mArchive != nullptr)
        {
            CUBOS_CRITICAL("Could not mount archive at root, an archive has already been mounted");
            abort();
        }

        if (!archive->isDirectory(1))
        {
            CUBOS_CRITICAL("Could not mount archive at root, archive must be a directory to be mounted");
            abort();
        }

        dir->mArchive = archive;
        dir->mId = 1;
        dir->generateArchive();
    }
    // Mount the archive as a child of 'dir'.
    else
    {
        if (dir->findChild(name) != nullptr)
        {
            CUBOS_CRITICAL("Could not mount archive at path '{}', a file already exists at that path",
                           dir->mPath + "/" + std::string(name));
            abort();
        }

        // Add mount point to the directory.
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), archive, name));
        file->generateArchive();
        file->mSibling = dir->mChild;
        dir->mChild = file;
    }

    CUBOS_TRACE("Mounted archive at path '{}' relative to '{}'", path, mPath);
}

void File::generateArchive()
{
    if (mArchive == nullptr || !mDirectory)
    {
        return;
    }

    auto child = mArchive->getChild(mId);
    while (child != 0)
    {
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), mArchive, child));
        file->mSibling = mChild;
        mChild = file;
        file->generateArchive();
        child = mArchive->getSibling(child);
    }
}

void File::unmount(std::string_view path)
{
    // Find mount point.
    auto mountPoint = this->find(path);
    if (mountPoint == nullptr)
    {
        CUBOS_ERROR("Could not unmount archive at path '{}', no mount point exists at that path", path);
        return;
    }

    // Lock the mount point mutex.
    std::lock_guard<std::mutex> lock(mountPoint->mMutex);

    // Check if the file is a mount point.
    if (mountPoint->mArchive == nullptr)
    {
        CUBOS_ERROR("Could not unmount archive at path '{}', no archive mounted on that path");
        return;
    }

    // Check if the mount point is the root directory of an archive.
    if (mountPoint->mParent != nullptr && mountPoint->mArchive == mountPoint->mParent->mArchive)
    {
        CUBOS_ERROR("Could not unmount archive at path '{}', the path must be the mount point (root) of an archive",
                    path);
        return;
    }

    // Recursively unmount all children.
    while (mountPoint->mChild)
    {
        mountPoint->mChild->destroyArchive();
        mountPoint->removeChild(mountPoint->mChild);
    }

    // Remove the mount point its parent directory.
    if (mountPoint->mParent != nullptr)
    {
        // Lock the parent directory mutex.
        std::lock_guard<std::mutex> dirLock(mountPoint->mParent->mMutex);
        mountPoint->mParent->removeChild(mountPoint);
        mountPoint->mParent = nullptr;
    }

    mountPoint->mArchive = nullptr;
    mountPoint->mId = 0;

    CUBOS_TRACE("Unmounted archive at path '{}' relative to '{}'", path, mPath);
}

void File::destroyArchive()
{
    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    if (mArchive != mParent->mArchive)
    {
        CUBOS_ERROR("Could not unmount archive, a file within the archive is mounted on a different archive, which "
                    "must be unmounted first");
        abort();
    }

    while (mChild)
    {
        mChild->destroyArchive();
        this->removeChild(mChild);
    }

    mParent = nullptr;
    mArchive = nullptr;
    mId = 0;
}

File::Handle File::find(std::string_view path)
{
    if (path.starts_with("./"))
    {
        path.remove_prefix(2);
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        return this->shared_from_this();
    }

    // If the path is absolute, or if this file isn't a directory, return nullptr.
    if ((!path.empty() && path[0] == '/') || !mDirectory)
    {
        return nullptr;
    }

    // Get name of the first component in the path.
    std::size_t i = 0;
    for (; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            break;
        }
    }
    auto name = path.substr(0, i);

    // Remove extra slashes.
    for (; i < path.size(); i++)
    {
        if (path[i] != '/')
        {
            break;
        }
    }

    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    // Search for the first child with the given name.
    auto child = this->findChild(name);
    if (child)
    {
        return child->find(path.substr(i));
    }
    return nullptr;
}

File::Handle File::create(std::string_view path, bool directory)
{
    if (path.starts_with("./"))
    {
        path.remove_prefix(2);
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        return this->shared_from_this();
    }

    // If the path is absolute, or if this file isn't a directory, return nullptr.
    if ((!path.empty() && path[0] == '/') || !mDirectory)
    {
        return nullptr;
    }

    // Get name of the first component in the path.
    std::size_t i = 0;
    for (; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            break;
        }
    }
    auto name = path.substr(0, i);

    // Remove extra slashes.
    for (; i < path.size(); i++)
    {
        if (path[i] != '/')
        {
            break;
        }
    }

    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);
    auto child = this->findChild(name);
    if (child == nullptr)
    {
        // Check if the directory is mounted on an archive.
        if (mArchive == nullptr)
        {
            CUBOS_ERROR("Could not create file at path '{}': parent directory must be mounted on an archive",
                        mPath + "/" + std::string(name));
            return nullptr;
        }

        // Check if the archive is read-only.
        if (mArchive->isReadOnly())
        {
            CUBOS_ERROR("Could not create file at path '{}': parent directory is mounted on a read-only archive",
                        mPath + "/" + std::string(name));
            return nullptr;
        }

        // Will the new file be a directory?
        bool childDirectory = path.size() > i || directory;

        // Create the file.
        std::size_t id = mArchive->create(mId, name, childDirectory);
        if (id == 0)
        {
            CUBOS_ERROR("Could not create file at path '{}': internal archive error", mPath + "/" + std::string(name));
            return nullptr;
        }

        // Add the file to this directory.
        child = std::shared_ptr<File>(new File(this->shared_from_this(), mArchive, id));
        this->addChild(child);
        CUBOS_TRACE("Created {} at path '{}'", childDirectory ? "directory" : "file", child->mPath);
    }

    return child->create(path.substr(i), directory);
}

bool File::destroy()
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mDestroyed)
    {
        return true;
    }
    mDestroyed = true;

    if (mArchive == nullptr)
    {
        CUBOS_ERROR("Could not destroy file '{}': file not mounted", mPath);
        return false;
    }
    if (mArchive->isReadOnly())
    {
        CUBOS_ERROR("Could not destroy file '{}': archive is read-only", mPath);
        return false;
    }
    if (mId == 1)
    {
        CUBOS_ERROR("Could not destroy file '{}': file is the mount point of an archive", mPath);
        return false;
    }

    // Lock the directory mutex and recursively destroy all children.
    while (mChild)
    {
        mChild->destroyRecursive();
        this->removeChild(mChild);
    }

    // Remove the file from the parent directory.
    std::lock_guard dirLock(mParent->mMutex);
    mParent->removeChild(this->shared_from_this());
    mParent = nullptr;

    CUBOS_TRACE("Destroyed file '{}'", mPath);
    return true;
}

void File::destroyRecursive()
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mDestroyed)
    {
        return;
    }

    // Mark the file as destroyed.
    mDestroyed = true;
    mParent = nullptr;

    // Recursively destroy all children.
    while (mChild)
    {
        mChild->destroyRecursive();
        this->removeChild(mChild);
    }
}

std::unique_ptr<memory::Stream> File::open(OpenMode mode)
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mArchive == nullptr)
    {
        CUBOS_WARN("Could not open file '{}': it is not mounted", mPath);
        return nullptr;
    }
    if (mArchive->isReadOnly() && mode == OpenMode::Write)
    {
        CUBOS_WARN("Could not open file '{}' for writing: archive is read-only", mPath);
        return nullptr;
    }
    if (mDirectory)
    {
        CUBOS_WARN("Could not open file '{}: it is a directory", mPath);
        return nullptr;
    }

    // Open the file.
    return mArchive->open(this->shared_from_this(), mode);
}

std::string_view File::getName() const
{
    return mName;
}

std::string_view File::getPath() const
{
    return mPath;
}

bool File::isDirectory() const
{
    return mDirectory;
}

std::shared_ptr<Archive> File::getArchive() const
{
    return mArchive;
}

std::size_t File::getId() const
{
    return mId;
}

File::Handle File::getParent() const
{
    return mParent;
}

File::Handle File::getSibling() const
{
    return mSibling;
}

File::Handle File::getChild() const
{
    return mChild;
}

void File::addChild(const File::Handle& child)
{
    child->mSibling = mChild;
    mChild = child;
}

void File::removeChild(const File::Handle& child)
{
    if (mChild == child)
    {
        mChild = child->mSibling;
    }
    else
    {
        auto prev = mChild;
        while (prev->mSibling != child)
        {
            prev = prev->mSibling;
        }
        prev->mSibling = child->mSibling;
    }
}

File::Handle File::findChild(std::string_view name) const
{
    for (auto child = mChild; child != nullptr; child = child->mSibling)
    {
        if (child->mName == name)
        {
            return child;
        }
    }
    return nullptr;
}

File::~File()
{
    if (mDestroyed && mArchive)
    {
        if (!mArchive->destroy(mId))
        {
            CUBOS_ERROR("Could not destroy file '{}': internal archive error", mPath);
        }
    }
}
