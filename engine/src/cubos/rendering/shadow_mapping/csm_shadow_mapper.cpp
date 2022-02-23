#include <cubos/rendering/shadow_mapping/csm_shadow_mapper.hpp>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

inline void CSMShadowMapper::setupFramebuffers()
{
    Texture2DArrayDesc atlasDesc;
    atlasDesc.format = TextureFormat::Depth32;
    atlasDesc.width = resolution;
    atlasDesc.height = resolution;

    atlasDesc.size = CUBOS_CSM_MAX_SPOT_SHADOW_COUNT;
    spotAtlas = renderDevice.createTexture2DArray(atlasDesc);

    atlasDesc.size = CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT * CUBOS_CSM_SHADOW_CASCADE_COUNT;
    directionalAtlas = renderDevice.createTexture2DArray(atlasDesc);

    FramebufferDesc spotFBDesc;
    spotFBDesc.targetCount = 0;
    spotFBDesc.depthStencil.setTexture2DArrayTarget(spotAtlas);
    spotFramebuffer = renderDevice.createFramebuffer(spotFBDesc);

    FramebufferDesc directionalFBDesc;
    directionalFBDesc.targetCount = 0;
    directionalFBDesc.depthStencil.setTexture2DArrayTarget(spotAtlas);
    directionalFramebuffer = renderDevice.createFramebuffer(directionalFBDesc);
}

inline void CSMShadowMapper::setupPipelines()
{
    
}

cubos::rendering::CSMShadowMapper::CSMShadowMapper(RenderDevice& renderDevice, size_t resolution)
    : ShadowMapper(renderDevice), resolution(resolution)
{
    setupFramebuffers();
    setupPipelines();
}

void cubos::rendering::CSMShadowMapper::bind()
{
}

void cubos::rendering::CSMShadowMapper::unbind()
{
}

void cubos::rendering::CSMShadowMapper::clear()
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::SpotLightData& light)
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::DirectionalLightData& light)
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::PointLightData& light)
{
}

size_t cubos::rendering::CSMShadowMapper::getSpotOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                        std::vector<glm::mat4>& matrices)
{
    return ShadowMapper::getSpotOutput(mapAtlas, matrices);
}

size_t cubos::rendering::CSMShadowMapper::getDirectionalOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                               std::vector<glm::mat4>& matrices,
                                                               size_t& atlasStride)
{
    return ShadowMapper::getDirectionalOutput(mapAtlas, matrices, atlasStride);
}
