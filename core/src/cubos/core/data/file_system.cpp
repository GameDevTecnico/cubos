#include <cubos/core/data/file_system.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

File::Handle FileSystem::root()
{
    static File::Handle root = File::Handle(new File(nullptr, ""));
    return root;
}

void FileSystem::mount(std::string_view path, std::shared_ptr<Archive> archive)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_CRITICAL("Could not mount archive at '{}': path must be absolute", path);
        abort();
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    FileSystem::root()->mount(path, archive);
}

void FileSystem::unmount(std::string_view path)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_ERROR("Could not unmount archive at '{}': path must be absolute", path);
        return;
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    FileSystem::root()->unmount(path);
}

File::Handle FileSystem::find(std::string_view path)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_ERROR("Could not find file at path '{}', the path must be absolute", path);
        return nullptr;
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    return FileSystem::root()->find(path);
}

File::Handle FileSystem::create(std::string_view path, bool directory)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_ERROR("Could not create file at path '{}', the path must be absolute", path);
        return nullptr;
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    return FileSystem::root()->create(path, directory);
}

bool FileSystem::destroy(std::string_view path)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_ERROR("Could not destroy file at path '{}', the path must be absolute", path);
        return false;
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    // Find the file.
    auto file = FileSystem::root()->find(path);
    if (file)
        return file->destroy();
    else
    {
        CUBOS_WARN("Could not destroy file at path '{}', the file does not exist", path);
        return false;
    }
}

std::unique_ptr<memory::Stream> FileSystem::open(std::string_view path, File::OpenMode mode)
{
    if (path.empty() || path[0] != '/')
    {
        CUBOS_ERROR("Could not open file at path '{}', the path must be absolute", path);
        return nullptr;
    }

    // Remove leading slashes.
    while (path.starts_with('/'))
        path.remove_prefix(1);

    // Find the file.
    auto file = FileSystem::root()->find(path);
    if (file)
        return file->open(mode);
    else
    {
        CUBOS_WARN("Could not open file at path '{}', the file does not exist", path);
        return nullptr;
    }
}
