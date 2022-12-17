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
        CUBOS_ERROR("No path specified for grid file");
        return nullptr;
    }

    auto file = core::data::FileSystem::find(path->second);
    if (!file)
    {
        CUBOS_ERROR("Could not find grid file '{}'", path->second);
        return nullptr;
    }

    auto stream = file->open(core::data::File::OpenMode::Read);
    if (!stream)
    {
        CUBOS_ERROR("Could not open grid file '{}'", path->second);
        return nullptr;
    }

    auto deserializer = core::data::BinaryDeserializer(*stream);
    auto asset = new Grid();
    deserializer.read(asset->grid);
    asset->rendererGrid = renderer->upload(asset->grid);
    return asset;
}

std::future<const void*> impl::GridLoader::loadAsync(const Meta& meta)
{
    return std::async(std::launch::async, [this, &meta] { return load(meta); });
}

void impl::GridLoader::unload(const Meta& meta, const void* asset)
{
    delete static_cast<const Grid*>(asset);
}
