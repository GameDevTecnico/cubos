#include <cubos/engine/data/palette.hpp>
#include <cubos/engine/data/asset_manager.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/binary_deserializer.hpp>

using namespace cubos;
using namespace cubos::engine::data;

const void* impl::PaletteLoader::load(const Meta& meta)
{
    auto path = meta.getParameters().find("path");
    if (path == meta.getParameters().end())
    {
        core::logError("PaletteLoader::load(): no path specified");
        return nullptr;
    }

    auto file = core::data::FileSystem::find(path->second);
    if (!file)
    {
        core::logError("PaletteLoader::load(): file '{}' not found", path->second);
        return nullptr;
    }

    auto stream = file->open(core::data::File::OpenMode::Read);
    if (!stream)
    {
        core::logError("PaletteLoader::load(): failed to open file '{}'", path->second);
        return nullptr;
    }

    auto deserializer = core::data::BinaryDeserializer(*stream);
    auto asset = new Palette();
    deserializer.read(asset->palette);
    return asset;
}

std::future<const void*> impl::PaletteLoader::loadAsync(const Meta& meta)
{
    return std::async(std::launch::async, [this, &meta] { return load(meta); });
}

void impl::PaletteLoader::unload(const Meta& meta, const void* asset)
{
    delete static_cast<const Palette*>(asset);
}
