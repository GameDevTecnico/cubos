#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/render/shader/bridge.hpp>

using namespace cubos::engine;

using cubos::core::memory::Stream;

bool ShaderBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    // Dump the shader code into a string.
    std::string contents;
    stream.readUntil(contents, nullptr);

    // Store the asset's data.
    assets.store(handle, Shader(mStage, mRenderDevice, contents));
    return true;
}

bool ShaderBridge::saveToFile(const Assets& assets, const AnyAsset& handle, cubos::core::memory::Stream& stream)
{
    // Ignore unused argument warnings
    (void)assets;
    (void)handle;
    (void)stream;

    CUBOS_ERROR("Shader assets are not saveable");
    return false;
}
