#include <cubos/engine/data/qb_model.hpp>

#include <cubos/core/data/file_system.hpp>

using namespace cubos;
using namespace cubos::engine::data;

const void* impl::QBModelLoader::load(const Meta& meta)
{
    auto path = meta.getParameters().find("path");
    if (path == meta.getParameters().end())
    {
        core::logError("QBModelLoader::load(): no path specified");
        return nullptr;
    }

    auto file = core::data::FileSystem::find(path->second);
    if (!file)
    {
        core::logError("QBModelLoader::load(): file '{}' not found", path->second);
        return nullptr;
    }

    auto stream = file->open(core::data::File::OpenMode::Read);
    if (!stream)
    {
        core::logError("QBModelLoader::load(): failed to open file '{}'", path->second);
        return nullptr;
    }

    std::vector<core::data::QBMatrix> matrices;
    if (!core::data::parseQB(matrices, *stream))
    {
        core::logError("QBModelLoader::load(): failed to parse QB file '{}'", path->second);
        return nullptr;
    }

    return new QBModel{std::move(matrices)};
}

std::future<const void*> impl::QBModelLoader::loadAsync(const Meta& meta)
{
    return std::async(std::launch::async, [this, &meta] { return load(meta); });
}

void impl::QBModelLoader::unload(const Meta& meta, const void* asset)
{
    delete static_cast<const QBModel*>(asset);
}
