#include <utility>

#include <cubos/core/data/archive.hpp>
#include <cubos/core/data/file.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

File::File(Handle parent, std::string_view name)
    : mName(name)
    , mDirectory(true)
    , mParent(std::move(parent))
{
    if (mParent)
    {
        mPath = mParent->mPath + "/" + std::string(mName);
    }
}

File::File(Handle parent, std::shared_ptr<Archive> archive, std::size_t id)
    : mArchive(std::move(archive))
    , mId(id)
    , mParent(std::move(parent))
{
    mName = mArchive->name(id);
    mDirectory = mArchive->directory(id);
    mPath = mParent->mPath + "/" + std::string(mName);
}

File::File(Handle parent, std::shared_ptr<Archive> archive, std::string_view name)
    : mName(name)
    , mArchive(std::move(archive))
    , mId(1)
    , mParent(std::move(parent))
{
    mDirectory = mArchive->directory(1);
    mPath = mParent->mPath + "/" + std::string(mName);
}

bool File::mount(std::string_view path, std::unique_ptr<Archive> archive)
{
    if (path.starts_with('/'))
    {
        CUBOS_ERROR("Could not mount archive at '{}' relative to '{}': path must be relative", path, mPath);
        return false;
    }
    else if (path.ends_with('/'))
    {
        CUBOS_ERROR("Could not mount archive at '{}' relative to '{}': path must not have trailing slashes", path,
                    mPath);
        return false;
    }

    // Split the path into directory and file.
    auto i = path.find_last_of('/');
    auto pathDir = i == std::string::npos ? "" : path.substr(0, i);
    auto pathName = i == std::string::npos ? path : path.substr(i + 1);

    // Check if the directory exists.
    auto dir = this->find(pathDir);
    if (!dir)
    {
        CUBOS_ERROR("Could not mount archive at '{}/{}': directory '{}/{}' does not exist", mPath, path, mPath,
                    pathDir);
        return false;
    }

    // Lock mutex of the directory.
    std::lock_guard<std::mutex> dirLock(dir->mMutex);

    // Mount the archive in the directory itself (must be the root directory).
    if (pathName.empty())
    {
        if (dir->mParent != nullptr)
        {
            CUBOS_ERROR("Could not mount archive at '{}': file already exists", dir->mPath);
            return false;
        }

        if (dir->mArchive != nullptr)
        {
            CUBOS_ERROR("Could not mount archive at root: it is already the mount point of another archive");
            return false;
        }

        if (!archive->directory(1))
        {
            CUBOS_ERROR("Could not mount archive at root: root mounted archives must be directory archives");
            return false;
        }

        if (dir->mChild != nullptr)
        {
            CUBOS_ERROR("Could not mount archive at root: root is not empty");
            return false;
        }

        dir->mArchive = std::move(archive);
        dir->mId = 1;
        dir->addArchive();
    }
    // Mount the archive as a child of 'dir'.
    else
    {
        if (dir->findChild(pathName) != nullptr)
        {
            CUBOS_ERROR("Could not mount archive at '{}/{}': file already exists", dir->mPath, pathName);
            return false;
        }

        // Add mount point to the directory.
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), std::move(archive), pathName));
        file->addArchive();
        file->mSibling = dir->mChild;
        dir->mChild = file;
    }

    CUBOS_INFO("Mounted archive at '{}/{}'", mPath, path);
    return true;
}

void File::addArchive()
{
    if (mArchive == nullptr || !mDirectory)
    {
        return;
    }

    // This is only called when an archive is mounted, so this directory should not have any files
    // yet.
    CUBOS_ASSERT(mChild == nullptr);

    // Create a child file for each child found in the archive.
    for (auto child = mArchive->child(mId); child != 0; child = mArchive->sibling(child))
    {
        auto file = std::shared_ptr<File>(new File(this->shared_from_this(), mArchive, child));
        file->mSibling = mChild;
        mChild = file;
        file->addArchive();
    }
}

bool File::unmount(std::string_view path)
{
    auto file = this->find(path);
    if (file == nullptr)
    {
        CUBOS_ERROR("Could not unmount archive at '{}/{}': no such file", mPath, path);
        return false;
    }

    // Lock the file mutex.
    std::lock_guard<std::mutex> lock(file->mMutex);

    if (file->mArchive == nullptr)
    {
        CUBOS_ERROR("Could not unmount archive at '{}/{}': file does not belong to an archive", mPath, path);
        return false;
    }

    // Is the file really the root of the archive?
    if (file->mId != 1)
    {
        CUBOS_ERROR("Could not unmount archive at '{}/{}': file is not the root of its archive", mPath, path);
        return false;
    }

    // Recursively unmount all children.
    while (file->mChild)
    {
        if (!file->mChild->removeArchive())
        {
            file->removeChild(file->mChild);
        }
    }

    // Remove the mount point from its parent directory if it is now empty.
    if (file->mParent != nullptr && file->mChild == nullptr)
    {
        // Lock the parent directory mutex.
        std::lock_guard<std::mutex> dirLock(file->mParent->mMutex);
        file->mParent->removeChild(file);
        file->mParent = nullptr;
    }

    file->mArchive = nullptr;
    file->mId = 0;

    CUBOS_INFO("Unmounted archive at '{}/{}'", mPath, path);
    return true;
}

bool File::removeArchive()
{
    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    if (mArchive != mParent->mArchive)
    {
        // Do not remove this file - it belongs to another archive from the one being unmounted.
        return true;
    }

    while (mChild)
    {
        if (!mChild->removeArchive())
        {
            this->removeChild(mChild);
        }
    }

    mArchive = nullptr;
    mId = 0;

    if (mChild == nullptr)
    {
        mParent = nullptr;
        return false;
    }

    return true;
}

File::Handle File::find(std::string_view path)
{
    if (path.starts_with('/'))
    {
        CUBOS_ERROR("Could not find file at '{}' relative to '{}': path must be relative", path, mPath);
        return nullptr;
    }
    else if (path.ends_with('/'))
    {
        CUBOS_ERROR("Could not find file at '{}' relative to '{}': path must not have trailing slashes", path, mPath);
        return nullptr;
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        return this->shared_from_this();
    }

    if (!mDirectory)
    {
        CUBOS_ERROR("Could not find file at '{}/{}': '{}' is not a directory", mPath, path, mPath);
        return nullptr;
    }

    // Get name of the first component in the path.
    auto i = path.find_first_of('/');
    auto childName = path.substr(0, i);
    auto pathRem = i == std::string::npos ? "" : path.substr(i + 1);

    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    // Search for the child with the given name.
    auto child = this->findChild(childName);
    if (child)
    {
        return child->find(pathRem);
    }

    CUBOS_TRACE("Could not find file at '{}/{}': no such file '{}/{}'", mPath, path, mPath, childName);
    return nullptr;
}

File::Handle File::create(std::string_view path, bool directory)
{
    if (path.starts_with('/'))
    {
        CUBOS_ERROR("Could not create file at '{}' relative to '{}': path must be relative", path, mPath);
        return nullptr;
    }
    else if (path.ends_with('/'))
    {
        CUBOS_ERROR("Could not create file at '{}' relative to '{}': path must not have trailing slashes", path, mPath);
        return nullptr;
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        // Make sure the file has the expected type.
        if (mDirectory != directory)
        {
            CUBOS_ERROR("Could not create file at '{}/{}': file already exists, but is not a {}", mPath, path,
                        directory ? "directory" : "regular file");
            return nullptr;
        }

        return this->shared_from_this();
    }

    if (!mDirectory)
    {
        CUBOS_ERROR("Could not create file at '{}/{}': '{}' is not a directory", mPath, path, mPath);
        return nullptr;
    }

    // Get name of the first component in the path.
    auto i = path.find_first_of('/');
    auto childName = path.substr(0, i);
    auto pathRem = i == std::string::npos ? "" : path.substr(i + 1);

    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    // Search for the child with the new file name.
    auto child = this->findChild(childName);
    if (child == nullptr)
    {
        // Check if the directory is mounted on an archive.
        if (mArchive == nullptr)
        {
            CUBOS_ERROR("Could not create file at '{}/{}': parent directory is not on an archive", mPath, childName);
            return nullptr;
        }

        // Check if the archive is read-only.
        if (mArchive->readOnly())
        {
            CUBOS_ERROR("Could not create file at '{}/{}': parent directory is on a read-only archive", mPath,
                        childName);
            return nullptr;
        }

        // The new file will be a directory if there is more path remaining or if the caller
        // requested a directory.
        auto childDirectory = !pathRem.empty() || directory;

        // Create the file.
        std::size_t id = mArchive->create(mId, childName, childDirectory);
        if (id == 0)
        {
            CUBOS_ERROR("Could not create file at '{}/{}': internal archive error", mPath, childName);
            return nullptr;
        }

        // Add the file to this directory.
        child = std::shared_ptr<File>(new File(this->shared_from_this(), mArchive, id));
        this->addChild(child);
        CUBOS_TRACE("Created {} '{}'", childDirectory ? "directory" : "file", child->mPath);
    }

    return child->create(pathRem, directory);
}

bool File::destroy()
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mDestroyed)
    {
        // If the file has already been marked as destroyed, do nothing.
        return true;
    }

    if (mArchive == nullptr)
    {
        CUBOS_ERROR("Could not destroy file '{}': file not on an archive", mPath);
        return false;
    }

    if (mArchive->readOnly())
    {
        CUBOS_ERROR("Could not destroy file '{}': file is on a read-only archive", mPath);
        return false;
    }

    if (mId == 1)
    {
        CUBOS_ERROR("Could not destroy file '{}': file is the root of an archive - did you want to use File::unmount?",
                    mPath);
        return false;
    }

    // Recursively destroy all children of this file, if any.
    while (mChild)
    {
        if (!mChild->destroyRecursive())
        {
            this->removeChild(mChild);
        }
    }

    // Only destroy the file if there are no more children.
    if (mChild == nullptr)
    {
        mDestroyed = true;

        // Remove the file from the parent directory.
        std::lock_guard dirLock(mParent->mMutex);
        mParent->removeChild(this->shared_from_this());
        mParent = nullptr;

        CUBOS_TRACE("Destroyed file '{}'", mPath);
        return true;
    }

    CUBOS_ERROR(
        "Could not destroy file '{}': file could only be partially destroyed due to an archive being mounted below it",
        mPath);
    return false;
}

bool File::destroyRecursive()
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mDestroyed)
    {
        return false;
    }

    // Recursively destroy all children.
    while (mChild)
    {
        if (!mChild->destroyRecursive())
        {
            this->removeChild(mChild);
        }
    }

    // Mark the file as destroyed if there are no more children.
    if (mChild == nullptr)
    {
        mDestroyed = true;
        mParent = nullptr;
        return false;
    }

    return true;
}

std::unique_ptr<memory::Stream> File::open(OpenMode mode)
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    if (mDirectory)
    {
        CUBOS_ERROR("Could not open file '{}: it is a directory", mPath);
        return nullptr;
    }

    if (mArchive == nullptr)
    {
        CUBOS_ERROR("Could not open file '{}': file is not on an archive", mPath);
        return nullptr;
    }

    if (mArchive->readOnly() && mode == OpenMode::Write)
    {
        CUBOS_ERROR("Could not open file '{}' for writing: file is on a read-only archive", mPath);
        return nullptr;
    }

    return mArchive->open(mId, this->shared_from_this(), mode);
}

std::string_view File::name() const
{
    return mName;
}

std::string_view File::path() const
{
    return mPath;
}

bool File::directory() const
{
    return mDirectory;
}

const std::shared_ptr<Archive>& File::archive() const
{
    return mArchive;
}

std::size_t File::id() const
{
    return mId;
}

File::Handle File::parent() const
{
    return mParent;
}

File::Handle File::sibling() const
{
    return mSibling;
}

File::Handle File::child() const
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
    if (mDestroyed && mArchive != nullptr)
    {
        if (!mArchive->destroy(mId))
        {
            CUBOS_ERROR("Could not destroy file '{}': internal archive error", mPath);
        }
    }
}
