#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/audio/audio.hpp>
#include <cubos/engine/audio/bridge.hpp>

using namespace cubos::engine;

bool AudioBridge::loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    Audio audio{mContext, stream};
    if (audio.buffer == nullptr)
    {
        return false;
    }
    assets.store(handle, std::move(audio));
    return true;
}
bool AudioBridge::saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    (void)assets;
    (void)handle;
    (void)stream;
    CUBOS_ERROR("Saving audios is currently unsupported");
    return false;
}
