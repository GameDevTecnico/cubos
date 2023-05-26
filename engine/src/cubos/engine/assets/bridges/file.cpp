#include <cubos/core/data/file_system.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

using namespace cubos::engine;
using cubos::core::data::File;
using cubos::core::data::FileSystem;

bool FileBridge::load(Assets& assets, const AnyAsset& handle)
{
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Read);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open file '{}'", path);
        return false;
    }

    return this->loadFromFile(assets, handle, *stream);
}

bool FileBridge::save(const Assets& assets, const AnyAsset& handle)
{
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Write);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open file '{}'", path);
        return false;
    }

    return this->saveToFile(assets, handle, *stream);
}
