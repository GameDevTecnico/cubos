#include <cubos/engine/voxels/palette.hpp>
#include <cubos/engine/voxels/palette_bridge.hpp>

using cubos::core::memory::Stream;
using cubos::engine::VoxelPalette;

using namespace cubos::engine;

PaletteBridge::PaletteBridge()
    : FileBridge(core::reflection::reflect<VoxelPalette>())
{
}

bool PaletteBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    VoxelPalette palette{};
    if (palette.loadFrom(stream))
    {
        assets.store(handle, std::move(palette));
        return true;
    }
    return false;
}

bool PaletteBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto palette = assets.read<VoxelPalette>(handle);
    return palette->writeTo(stream);
}