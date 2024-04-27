#include <utility>

#include <cubos/core/data/fs/archive.hpp>
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string_view.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

File::Handle FileSystem::root()
{
    static auto root = std::shared_ptr<File>(new File(nullptr, ""));
    return root;
}

bool FileSystem::mount(std::string_view path, std::unique_ptr<Archive> archive)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not mount archive at {}: path must be absolute", path);
        return false;
    }

    path.remove_prefix(1);
    return FileSystem::root()->mount(path, std::move(archive));
}

bool FileSystem::unmount(std::string_view path)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not mount archive at {}: path must be absolute", path);
        return false;
    }

    path.remove_prefix(1);
    return FileSystem::root()->unmount(path);
}

File::Handle FileSystem::find(std::string_view path)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not find file at {}: path must be absolute", path);
        return nullptr;
    }

    path.remove_prefix(1);
    return FileSystem::root()->find(path);
}

File::Handle FileSystem::create(std::string_view path, bool directory)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not create file at {}: path must be absolute", path);
        return nullptr;
    }

    path.remove_prefix(1);
    return FileSystem::root()->create(path, directory);
}

bool FileSystem::destroy(std::string_view path)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not destroy file at {}: path must be absolute", path);
        return false;
    }

    path.remove_prefix(1);
    if (auto file = FileSystem::root()->find(path))
    {
        return file->destroy();
    }

    CUBOS_ERROR("Could not destroy file at '/{}': no such file", path);
    return false;
}

std::unique_ptr<memory::Stream> FileSystem::open(std::string_view path, File::OpenMode mode)
{
    if (!path.starts_with('/'))
    {
        CUBOS_ERROR("Could not open file at {}: path must be absolute", path);
        return nullptr;
    }

    path.remove_prefix(1);

    if (mode == File::OpenMode::Read)
    {
        if (auto file = FileSystem::root()->find(path))
        {
            return file->open(mode);
        }

        CUBOS_ERROR("Could not open file for reading at path {}: the file does not exist", path);
        return nullptr;
    }

    if (auto file = FileSystem::root()->create(path))
    {
        return file->open(mode);
    }

    CUBOS_ERROR("Could not open file for writing at path {}: the file could not be created", path);
    return nullptr;
}

bool FileSystem::copy(std::string_view sourcePath, std::string_view destinationPath)
{
    auto srcStream = FileSystem::open(sourcePath, File::OpenMode::Read);

    if (srcStream == nullptr)
    {
        CUBOS_ERROR("Could not open source file {} for copying", sourcePath);
        return false;
    }

    auto dstFile = FileSystem::create(destinationPath);

    if (dstFile == nullptr)
    {
        CUBOS_ERROR("Could not create destination file {} for copying", destinationPath);
        return false;
    }

    auto dstStream = dstFile->open(File::OpenMode::Write);

    if (dstStream == nullptr)
    {
        CUBOS_ERROR("Could not open destination file {} for copying", destinationPath);
        return false;
    }

    constexpr std::size_t ChunkSize = 4096;
    char buffer[ChunkSize];

    while (!srcStream->eof())
    {
        std::size_t bytesRead = srcStream->read(buffer, ChunkSize);

        if (bytesRead > 0)
        {
            dstStream->write(buffer, bytesRead);
        }
    }

    return true;
}

bool FileSystem::move(std::string_view sourcePath, std::string_view destinationPath)
{
    if (FileSystem::find(destinationPath) == nullptr)
    {
        CUBOS_ERROR("File destination {} already exists, can not move", destinationPath);
        return false;
    }

    auto file = FileSystem::find(sourcePath);
    if (file == nullptr)
    {
        CUBOS_ERROR("Could not find file {} for moving", sourcePath);
        return false;
    }

    if (!FileSystem::copy(sourcePath, destinationPath))
    {
        CUBOS_ERROR("Could not copy file {} to {} for moving", sourcePath, destinationPath);
        return false;
    }

    if (!file->destroy())
    {
        CUBOS_ERROR("Could not destroy file {} after moving", sourcePath, destinationPath);
        return false;
    }

    return true;
}