#include <cubos/core/data/embedded_archive.hpp>
#include <cubos/core/data/file_stream.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

EmbeddedArchive::EmbeddedArchive(const std::string& name)
{
    auto& registry = EmbeddedArchive::getRegistry();
    auto it = registry.find(name);
    if (it == registry.end())
    {
        CUBOS_CRITICAL("No embedded archive with name '{}' found", name);
        abort();
    }

    mData = &it->second;
}

std::map<std::string, const EmbeddedArchive::Data&>& EmbeddedArchive::getRegistry()
{
    static std::map<std::string, const EmbeddedArchive::Data&> registry;
    return registry;
}

void EmbeddedArchive::registerData(const std::string& name, const Data& data)
{
    auto& registry = EmbeddedArchive::getRegistry();
    if (registry.find(name) != registry.end())
    {
        CUBOS_CRITICAL("Embedded archive with name '{}' already registered", name);
        abort();
    }

    registry.emplace(name, data);
}

std::size_t EmbeddedArchive::create(std::size_t /*parent*/, std::string_view /*name*/, bool /*directory*/)
{
    // Embedded archive is read-only.
    return 0;
}

bool EmbeddedArchive::destroy(std::size_t /*id*/)
{
    // Embedded archive is read-only.
    return false;
}

std::string EmbeddedArchive::getName(std::size_t id) const
{
    return mData->entries[id - 1].name;
}

bool EmbeddedArchive::isDirectory(std::size_t id) const
{
    return mData->entries[id - 1].isDirectory;
}

bool EmbeddedArchive::isReadOnly() const
{
    return true;
}

std::size_t EmbeddedArchive::getParent(std::size_t id) const
{
    return mData->entries[id - 1].parent;
}

std::size_t EmbeddedArchive::getSibling(std::size_t id) const
{
    return mData->entries[id - 1].sibling;
}

std::size_t EmbeddedArchive::getChild(std::size_t id) const
{
    return mData->entries[id - 1].child;
}

std::unique_ptr<memory::Stream> EmbeddedArchive::open(File::Handle file, File::OpenMode mode)
{
    if (mode != File::OpenMode::Read)
    {
        return nullptr;
    }

    const auto& entry = mData->entries[file->getId() - 1];
    return std::make_unique<FileStream<memory::BufferStream>>(file, mode, memory::BufferStream(entry.data, entry.size));
}
