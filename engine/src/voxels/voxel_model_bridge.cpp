#include <cubos/engine/voxels/voxel_model.hpp>
#include <cubos/engine/voxels/voxel_model_bridge.hpp>

using cubos::core::memory::Stream;
using cubos::engine::VoxelModel;

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
        assets.store(handle, std::move(model));
        return true;
    }
    return false;
}

bool VoxelModelBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto model = assets.read<VoxelModel>(handle);
    return model->writeTo(stream);
}