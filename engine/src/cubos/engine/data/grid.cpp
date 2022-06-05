#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/asset_manager.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/binary_deserializer.hpp>

using namespace cubos;
using namespace cubos::engine::data;

impl::GridLoader::GridLoader(AssetManager* manager, gl::Renderer* renderer) : Loader(manager), renderer(renderer)
{
    // Do nothing.
}

const void* impl::GridLoader::load(const Meta& meta)
{
    auto path = meta.getParameters().find("path");
    if (path == meta.getParameters().end())
    {
        core::logError("GridLoader::load(): no path specified");
        return nullptr;
    }

    auto file = core::data::FileSystem::find(path->second);
    if (!file)
    {
        core::logError("GridLoader::load(): file '{}' not found", path->second);
        return nullptr;
    }

    auto stream = file->open(core::data::File::OpenMode::Read);
    if (!stream)
    {
        core::logError("GridLoader::load(): failed to open file '{}'", path->second);
        return nullptr;
    }

    auto deserializer = core::data::BinaryDeserializer(*stream);
    auto asset = new Grid();
    deserializer.read(asset->grid);
    asset->id = renderer->upload(asset->grid);
    return asset;
}

std::future<const void*> impl::GridLoader::loadAsync(const Meta& meta)
{
    return std::async(std::launch::async, [this, &meta] { return load(meta); });
}

void impl::GridLoader::unload(const Meta& meta, const void* asset)
{
    renderer->free(static_cast<const Grid*>(asset)->id);
    delete static_cast<const Grid*>(asset);
}
