#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/bridges/file.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;

using namespace cubos::engine;

bool FileBridge::load(Assets& assets, const AnyAsset& handle)
{
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Read);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open file {}", path);
        return false;
    }

    if (!this->loadFromFile(assets, handle, *stream))
    {
        CUBOS_ERROR("Could not load asset from file {}", path);
        return false;
    }

    return true;
}

bool FileBridge::save(const Assets& assets, const AnyAsset& handle)
{
    auto path = assets.readMeta(handle)->get("path").value();

    auto swpPath = path + ".swp";
    auto swpFile = FileSystem::create(swpPath);

    if (swpFile == nullptr)
    {
        CUBOS_ERROR("Could not create swap file {}", swpPath);
        return false;
    }

    auto stream = swpFile->open(File::OpenMode::Write);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open swap file {}", swpPath);
        return false;
    }

    if (!this->saveToFile(assets, handle, *stream))
    {
        CUBOS_ERROR("Could not save asset to swap file");
        return false;
    }

    // Close stream, ensuring everything is flushed to the file.
    stream = nullptr;

    /// @todo This can be done simpler with #737.
    if (!FileSystem::copy(swpPath, path))
    {
        CUBOS_ERROR("Could not overwrite asset {} with swap file", path);
        return false;
    }

    if (!swpFile->destroy())
    {
        CUBOS_ERROR("Could not destroy swap asset file");
        return false;
    }

    return true;
}
