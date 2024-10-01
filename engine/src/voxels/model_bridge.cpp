#include <cubos/engine/voxels/model.hpp>
#include <cubos/engine/voxels/model_bridge.hpp>

using cubos::core::memory::Stream;

using namespace cubos::engine;

VoxelModelBridge::VoxelModelBridge()
    : FileBridge(core::reflection::reflect<VoxelModel>())
{
}

bool VoxelModelBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    VoxelModel model{};
    if (model.loadFrom(stream))
    {
        assets.store(handle, model);
        return true;
    }
    return false;
}

bool VoxelModelBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto model = assets.read<VoxelModel>(handle);
    return model->writeTo(stream);
}