#include <utility>

#include <cubos/core/data/fs/archive.hpp>
#include <cubos/core/data/fs/file.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

static bool validateRelativePath(std::string_view path)
{
    if (path.empty())
    {
        return true;
    }

    auto remPath = path;
    while (!remPath.empty())
    {
        auto i = remPath.find_first_of('/');
        if (i == std::string::npos)
        {
            return true;
        }

        if (i == 0)
        {
            break;
        }

        remPath = remPath.substr(i + 1);
    }

    CUBOS_ERROR("Invalid relative path {}: cannot have leading, trailing or consecutive slashes", path);
    return false;
}

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
    if (!validateRelativePath(path))
    {
        CUBOS_ERROR("Could not mount archive at {} relative to {}: invalid relative path", path, mPath);
        return false;
    }

    // Lock mutex of the file.
    std::lock_guard<std::mutex> dirLock(mMutex);

    if (mArchive != nullptr)
    {
        CUBOS_ERROR("Could not mount archive at {}/{}: {} is already part of an archive", mPath, path, mPath);
        return false;
    }

    // Get name of the first component in the path.
    auto i = path.find_first_of('/');
    auto childName = path.substr(0, i);
    auto pathRem = i == std::string::npos ? "" : path.substr(i + 1);

    if (childName.empty())
    {
        // Since the path could not start with a slash, this means that the path is empty.
        CUBOS_ERROR("Could not mount archive at {}: file already exists", mPath);
        return false;
    }

    // Check if the child already exists.
    if (auto child = this->findChild(childName))
    {
        // If the child already exists, then just recurse into it.
        return child->mount(pathRem, std::move(archive));
    }

    // Check if we are at the end of the path.
    if (!pathRem.empty())
    {
        // If we are not at the end of the path, then we create a new directory and recurse into
        // it.
        auto dir = std::shared_ptr<File>(new File(this->shared_from_this(), childName));
        dir->mSibling = this->mChild;
        this->mChild = dir;
        return dir->mount(pathRem, std::move(archive));
    }

    // Otherwise the archive should be mounted as a child of this directory.
    auto file = std::shared_ptr<File>(new File(this->shared_from_this(), std::move(archive), childName));
    file->addArchive();
    file->mSibling = this->mChild;
    this->mChild = file;
    CUBOS_INFO("Mounted archive at {}/{}", mPath, childName);
    return true;
}

void File::addArchive()
{
    // This is only called when an archive is being mounted, so the file should not already have
    // a children.
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
        CUBOS_ERROR("Could not unmount archive at {}/{}: no such file", mPath, path);
        return false;
    }

    // Lock the file mutex.
    file->mMutex.lock();

    if (file->mArchive == nullptr)
    {
        file->mMutex.unlock();
        CUBOS_ERROR("Could not unmount archive at {}/{}: file does not belong to an archive", mPath, path);
        return false;
    }

    // Is the file really the root of the archive?
    if (file->mId != 1)
    {
        file->mMutex.unlock();
        CUBOS_ERROR("Could not unmount archive at {}/{}: file is not the root of its archive", mPath, path);
        return false;
    }

    // Recursively unmount all children.
    while (file->mChild != nullptr)
    {
        file->mChild->removeArchive();
        file->removeChild(file->mChild);
    }

    // Remove the mount point and any of its non-root parent directories which become empty.
    while (file->mParent != nullptr && file->mChild == nullptr)
    {
        auto next = file->mParent;

        // Remove the file from its parent directory.
        next->mMutex.lock();
        next->removeChild(file);
        file->mParent = nullptr;
        file->mMutex.unlock();
        file = next;
    }

    file->mMutex.unlock();

    CUBOS_INFO("Unmounted archive at {}/{}", mPath, path);
    return true;
}

void File::removeArchive()
{
    // Lock the mutex of this file.
    std::lock_guard<std::mutex> lock(mMutex);

    mArchive = nullptr;
    mId = 0;
    mParent = nullptr;

    // Recursively unmount all children.
    while (mChild != nullptr)
    {
        mChild->removeArchive();
        this->removeChild(mChild);
    }
}

File::Handle File::find(std::string_view path)
{
    if (!validateRelativePath(path))
    {
        CUBOS_ERROR("Could not find file at {} relative to {}: invalid relative path", path, mPath);
        return nullptr;
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        return this->shared_from_this();
    }

    if (!mDirectory)
    {
        CUBOS_ERROR("Could not find file at {}/{}: {} is not a directory", mPath, path, mPath);
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

    CUBOS_TRACE("Could not find file at {}/{}: no such file {}/{}", mPath, path, mPath, childName);
    return nullptr;
}

File::Handle File::create(std::string_view path, bool directory)
{
    if (!validateRelativePath(path))
    {
        CUBOS_ERROR("Could not create file at {} relative to {}: invalid relative path", path, mPath);
        return nullptr;
    }

    // If the path is empty, return this file.
    if (path.empty())
    {
        // Make sure the file has the expected type.
        if (mDirectory != directory)
        {
            CUBOS_ERROR("Could not create file at {}/{}: file already exists, but is not a {}", mPath, path,
                        directory ? "directory" : "regular file");
            return nullptr;
        }

        return this->shared_from_this();
    }

    if (!mDirectory)
    {
        CUBOS_ERROR("Could not create file at {}/{}: {} is not a directory", mPath, path, mPath);
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
            CUBOS_ERROR("Could not create file at {}/{}: parent directory is not on an archive", mPath, childName);
            return nullptr;
        }

        // Check if the archive is read-only.
        if (mArchive->readOnly())
        {
            CUBOS_ERROR("Could not create file at {}/{}: parent directory is on a read-only archive", mPath, childName);
            return nullptr;
        }

        // The new file will be a directory if there is more path remaining or if the caller
        // requested a directory.
        auto childDirectory = !pathRem.empty() || directory;

        // Create the file.
        std::size_t id = mArchive->create(mId, childName, childDirectory);
        if (id == 0)
        {
            CUBOS_ERROR("Could not create file at {}/{}: internal archive error", mPath, childName);
            return nullptr;
        }

        // Add the file to this directory.
        child = std::shared_ptr<File>(new File(this->shared_from_this(), mArchive, id));
        this->addChild(child);
        CUBOS_TRACE("Created {} {}", childDirectory ? "directory" : "file", child->mPath);
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
        CUBOS_ERROR("Could not destroy file {}: file not on an archive", mPath);
        return false;
    }

    if (mArchive->readOnly())
    {
        CUBOS_ERROR("Could not destroy file {}: file is on a read-only archive", mPath);
        return false;
    }

    if (mId == 1)
    {
        CUBOS_ERROR("Could not destroy file {}: file is the root of an archive - did you want to use File::unmount?",
                    mPath);
        return false;
    }

    mDestroyed = true;

    // Recursively destroy all children of this file, if any.
    while (mChild != nullptr)
    {
        mChild->destroyRecursive();
        this->removeChild(mChild);
    }

    // Remove the file from the parent directory.
    std::lock_guard dirLock(mParent->mMutex);
    mParent->removeChild(this->shared_from_this());
    mParent = nullptr;

    CUBOS_TRACE("Marked file {} for destruction", mPath);
    return true;
}

void File::destroyRecursive()
{
    // Lock the file mutex.
    std::lock_guard fileLock(mMutex);

    // Mark the file as destroyed.
    mDestroyed = true;

    // Recursively destroy all children and remove them from this file.
    // Do not set the parent of the children to null, as this could cause the parent file being
    // destroyed before all children have been destroyed.
    while (mChild != nullptr)
    {
        mChild->destroyRecursive();
        this->removeChild(mChild);
    }
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

    CUBOS_ASSERT(mArchive != nullptr, "Regular files must be on an archive");

    if (mArchive->readOnly() && mode != OpenMode::Read)
    {
        CUBOS_ERROR("Could not open file {} for writing: file is on a read-only archive", mPath);
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
    std::lock_guard lock(mMutex);
    return mArchive;
}

std::size_t File::id() const
{
    std::lock_guard lock(mMutex);
    return mId;
}

File::Handle File::parent() const
{
    std::lock_guard lock(mMutex);
    return mParent;
}

File::Handle File::sibling() const
{
    std::lock_guard lock(mMutex);
    return mSibling;
}

File::Handle File::child() const
{
    std::lock_guard lock(mMutex);
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
            CUBOS_ERROR("Could not destroy file {}: internal archive error", mPath);
        }
    }
}
