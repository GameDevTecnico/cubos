#include <cubos/core/log.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/image/bridge.hpp>
#include <cubos/engine/image/image.hpp>

using namespace cubos::engine;

bool ImageBridge::loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    Image image{stream};
    if (image.data == nullptr)
    {
        return false;
    }
    assets.store(handle, std::move(image));
    return true;
}

bool ImageBridge::saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    (void)assets;
    (void)handle;
    (void)stream;
    CUBOS_ERROR("Saving images is currently unsupported");
    return false;
}
