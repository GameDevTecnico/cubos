#include <cubos/core/gl/util.hpp>

#include <cubos/engine/renderer/pps/copy_pass.hpp>

using namespace cubos::core::gl;
using cubos::engine::PostProcessingCopy;
using cubos::engine::PostProcessingInput;

/// The vertex shader of the copy pass.
static const char* copyVs = R"glsl(
#version 330 core

in vec4 position;
in vec2 uv;

out vec2 fragUv;

void main(void)
{
    gl_Position = position;
    fragUv = uv;
}
)glsl";

/// The pixel shader of the copy pass.
static const char* copyPs = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D inputTex;

out vec4 color;

void main()
{
    color = texture(inputTex, fragUv);
}
)glsl";

PostProcessingCopy::PostProcessingCopy(RenderDevice& renderDevice, glm::uvec2 size)
    : PostProcessingPass(renderDevice)
    , mSize(size)
{
    // Create the shader pipeline.
    auto vs = mRenderDevice.createShaderStage(Stage::Vertex, copyVs);
    auto ps = mRenderDevice.createShaderStage(Stage::Pixel, copyPs);
    mPipeline = mRenderDevice.createShaderPipeline(vs, ps);
    mInputTexBp = mPipeline->getBindingPoint("inputTex");

    // Create the screen quad VA.
    generateScreenQuad(mRenderDevice, mPipeline, mScreenQuadVa);

    // Create the sampler.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    mInputTexSampler = mRenderDevice.createSampler(samplerDesc);
}

void PostProcessingCopy::resize(glm::uvec2 size)
{
    mSize = size;
}

void PostProcessingCopy::execute(std::map<PostProcessingInput, core::gl::Texture2D>& /*inputs*/,
                                 core::gl::Texture2D prev, core::gl::Framebuffer out) const
{
    // Set the framebuffer and state.
    mRenderDevice.setFramebuffer(out);
    mRenderDevice.setViewport(0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y));
    mRenderDevice.setRasterState(nullptr);
    mRenderDevice.setBlendState(nullptr);
    mRenderDevice.setDepthStencilState(nullptr);

    // Set the pipeline and draw the screen quad.
    mRenderDevice.setShaderPipeline(mPipeline);
    mInputTexBp->bind(prev);
    mInputTexBp->bind(mInputTexSampler);
    mRenderDevice.setVertexArray(mScreenQuadVa);
    mRenderDevice.drawTriangles(0, 6);
}
