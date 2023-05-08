#include <cubos/core/data/binary_deserializer.hpp>
#include <cubos/core/data/binary_serializer.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/assets/binary_bridge.hpp>

using cubos::core::data::BinaryDeserializer;
using cubos::core::data::BinarySerializer;
using cubos::core::data::File;
using cubos::core::data::FileSystem;
using namespace cubos::engine;

bool BinaryBridge::load(Assets& assets, const AnyAsset& handle)
{
    // Open the file.
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Read);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open file '{}'", path);
        return false;
    }

    // Deserialize the asset from the file stream.
    BinaryDeserializer deserializer{*stream};
    this->loadImpl(assets, handle, deserializer);
    if (deserializer.failed())
    {
        CUBOS_ERROR("Could not binary deserialize file '{}'", path);
        return false;
    }

    return true;
}

bool BinaryBridge::save(const Assets& assets, const AnyAsset& handle)
{
    // Open the file.
    auto path = assets.readMeta(handle)->get("path").value();
    auto stream = FileSystem::open(path, File::OpenMode::Write);
    if (stream == nullptr)
    {
        CUBOS_ERROR("Could not open file '{}'", path);
        return false;
    }

    // Serialize the asset to the file stream.
    BinarySerializer serializer{*stream};
    this->saveImpl(assets, handle, serializer);
    if (serializer.failed())
    {
        CUBOS_ERROR("Could not binary serialize asset to file '{}'", path);
        return false;
    }

    return true;
}
