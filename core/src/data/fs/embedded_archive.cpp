#include <cubos/core/data/fs/embedded_archive.hpp>
#include <cubos/core/data/fs/file_stream.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

EmbeddedArchive::EmbeddedArchive(const std::string& name)
{
    auto& registry = EmbeddedArchive::registry();
    auto it = registry.find(name);
    CUBOS_ASSERT(it != registry.end(), "No embedded archive with name '{}' found", name);
    mData = &it->second;
}

std::map<std::string, const EmbeddedArchive::Data&>& EmbeddedArchive::registry()
{
    static std::map<std::string, const EmbeddedArchive::Data&> registry;
    return registry;
}

void EmbeddedArchive::registerData(const std::string& name, const Data& data)
{
    auto& registry = EmbeddedArchive::registry();
    CUBOS_ASSERT(registry.find(name) == registry.end(), "Embedded archive with name '{}' already registered", name);
    registry.emplace(name, data);
}

std::size_t EmbeddedArchive::create(std::size_t /*parent*/, std::string_view /*name*/, bool /*directory*/)
{
    CUBOS_UNREACHABLE("Embedded archive is read-only");
}

bool EmbeddedArchive::destroy(std::size_t /*id*/)
{
    CUBOS_UNREACHABLE("Embedded archive is read-only");
}

std::string EmbeddedArchive::name(std::size_t id) const
{
    CUBOS_DEBUG_ASSERT(id > 0);
    return mData->entries[id - 1].name;
}

bool EmbeddedArchive::directory(std::size_t id) const
{
    CUBOS_DEBUG_ASSERT(id > 0);
    return mData->entries[id - 1].isDirectory;
}

bool EmbeddedArchive::readOnly() const
{
    return true;
}

std::size_t EmbeddedArchive::parent(std::size_t id) const
{
    CUBOS_DEBUG_ASSERT(id > 0);
    return mData->entries[id - 1].parent;
}

std::size_t EmbeddedArchive::sibling(std::size_t id) const
{
    CUBOS_DEBUG_ASSERT(id > 0);
    return mData->entries[id - 1].sibling;
}

std::size_t EmbeddedArchive::child(std::size_t id) const
{
    CUBOS_DEBUG_ASSERT(id > 0);
    return mData->entries[id - 1].child;
}

std::unique_ptr<memory::Stream> EmbeddedArchive::open(std::size_t id, File::Handle handle, File::OpenMode mode)
{
    CUBOS_DEBUG_ASSERT(mode == File::OpenMode::Read);

    const auto& entry = mData->entries[id - 1];
    return std::make_unique<FileStream<memory::BufferStream>>(handle, mode,
                                                              memory::BufferStream(entry.data, entry.size));
}
