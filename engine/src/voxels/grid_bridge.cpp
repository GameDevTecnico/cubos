#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/grid_bridge.hpp>

using cubos::core::memory::Stream;
using cubos::engine::VoxelGrid;

using namespace cubos::engine;

VoxelGridBridge::VoxelGridBridge()
    : FileBridge(core::reflection::reflect<VoxelGrid>())
{
}

bool VoxelGridBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    VoxelGrid grid{};
    if (grid.loadFrom(stream))
    {
        assets.store(handle, std::move(grid));
        return true;
    }
    return false;
}

bool VoxelGridBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto grid = assets.read<VoxelGrid>(handle);
    return grid->writeTo(stream);
}
