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
    ShaderStage vertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
        #version 460 core
        layout (location = 0) in uvec3 aPos;

        uniform M
        {
            mat4 M;
        };

        void main()
        {
            gl_Position = M * vec4(position, 1.0);
        }
    )");

    ShaderStage directionalGeom = renderDevice.createShaderStage(gl::Stage::Geometry, R"(
        #version 460 core

        layout(triangles, invocations = 5) in;
        layout(triangle_strip, max_vertices = 3) out;

        layout (std140) uniform LightSpaceMatrices
        {
            mat4 lightSpaceMatrices[8 * 5];
        };

        uniform uint atlasOffset;

        void main()
        {
            for (int i = 0; i < 3; ++i)
            {
                gl_Position = lightSpaceMatrices[atlasOffset + gl_InvocationID] * gl_in[i].gl_Position;
                gl_Layer = atlasOffset + gl_InvocationID;
                EmitVertex();
            }
            EndPrimitive();
        }
    )");

    ShaderStage pixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
        #version 460 core

        void main()
        {
        }
    )");
}

cubos::rendering::CSMShadowMapper::CSMShadowMapper(RenderDevice& renderDevice, size_t resolution)
    : ShadowMapper(renderDevice), resolution(resolution)
{
    setupFramebuffers();
    setupPipelines();
}

void CSMShadowMapper::setModelMatrix(glm::mat4 modelMat)
{
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
                                                               std::vector<glm::mat4>& matrices, size_t& atlasStride)
{
    return ShadowMapper::getDirectionalOutput(mapAtlas, matrices, atlasStride);
}
