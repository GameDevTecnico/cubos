#include <cubos/core/log.hpp>

#include <cubos/engine/render/shader/bridge.hpp>

using namespace cubos::engine;

using cubos::core::memory::Stream;

bool ShaderBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    // Dump the shader code into a string.
    std::string contents;
    stream.readUntil(contents, nullptr);

    // Read shader stage type
    auto stageTypeString = assets.readMeta(handle)->get("stage");
    cubos::core::gl::Stage stageType;
    if (stageTypeString == "Vertex")
    {
        stageType = cubos::core::gl::Stage::Vertex;
    }
    else if (stageTypeString == "Geometry")
    {
        stageType = cubos::core::gl::Stage::Geometry;
    }
    else if (stageTypeString == "Pixel")
    {
        stageType = cubos::core::gl::Stage::Pixel;
    }
    else if (stageTypeString == "Compute")
    {
        stageType = cubos::core::gl::Stage::Compute;
    }
    else
    {
        CUBOS_ERROR("Shader asset metadata must have a 'stage' property set to one of ''Vertex', 'Geometry', 'Pixel' "
                    "or 'Compute'");
        return false;
    }

    cubos::core::gl::ShaderStage shaderStage = mRenderDevice.createShaderStage(stageType, contents.c_str());
    if (shaderStage == nullptr)
    {
        CUBOS_ERROR("Shader asset stage creation failed");
        return false;
    }

    // Store the asset's data.
    assets.store(handle, Shader(shaderStage));
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
