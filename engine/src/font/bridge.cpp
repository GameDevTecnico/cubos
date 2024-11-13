#include <utility>

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/font/bridge.hpp>
#include <cubos/engine/font/font.hpp>

using namespace cubos::engine;

FontBridge::FontBridge()
    : FileBridge(core::reflection::reflect<Font>())
    , mFreetypeHandle(msdfgen::initializeFreetype())
{
}

FontBridge::~FontBridge()
{
    msdfgen::deinitializeFreetype(static_cast<msdfgen::FreetypeHandle*>(mFreetypeHandle));
}

bool FontBridge::loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    Font font{mFreetypeHandle, stream};
    if (font.handle() == nullptr)
    {
        return false;
    }
    assets.store(handle, std::move(font));
    return true;
}

bool FontBridge::saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream)
{
    (void)assets;
    (void)handle;
    (void)stream;
    CUBOS_ERROR("Saving fonts is currently unsupported!");
    return false;
}
