#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/asset_manager.hpp>

using namespace cubos;
using namespace cubos::engine::data;

const void* impl::GridLoader::load(const Meta& meta)
{
    auto qb = meta.getParameters().find("qb");
    if (qb == meta.getParameters().end())
    {
        core::logError("GridLoader::load(): no qb specified");
        return nullptr;
    }

    auto index_s = meta.getParameters().find("index");
    if (index_s == meta.getParameters().end())
    {
        core::logError("GridLoader::load(): no index specified");
        return nullptr;
    }

    auto index = std::stoi(index_s->second);

    auto model = this->manager->load<QBModel>(qb->second);
    if (!model)
    {
        core::logError("GridLoader::load(): no such asset '{}'", qb->second);
        return nullptr;
    }

    if (index >= model->matrices.size())
    {
        core::logError("GridLoader::load(): Qubicle model '{}' has no such index {} (>= {})", qb->second, index, model->matrices.size());
        return nullptr;
    }
    
    auto asset = new Grid();
    asset->grid = model->matrices[index].grid;
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
