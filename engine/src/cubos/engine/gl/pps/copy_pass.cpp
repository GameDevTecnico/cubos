#include <cubos/core/gl/util.hpp>

#include <cubos/engine/gl/pps/copy_pass.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

/// The vertex shader of the copy pass.
static const char* COPY_VS = R"glsl(
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
static const char* COPY_PS = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D inputTex;

out vec4 color;

void main()
{
    color = texture(inputTex, fragUv);
}
)glsl";

pps::CopyPass::CopyPass(RenderDevice& renderDevice, glm::uvec2 size)
    : Pass(renderDevice)
    , size(size)
{
    // Create the shader pipeline.
    auto vs = this->renderDevice.createShaderStage(Stage::Vertex, COPY_VS);
    auto ps = this->renderDevice.createShaderStage(Stage::Pixel, COPY_PS);
    this->pipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->inputTexBP = this->pipeline->getBindingPoint("inputTex");

    // Create the screen quad VA.
    generateScreenQuad(this->renderDevice, this->pipeline, this->screenQuadVA);

    // Create the sampler.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    this->inputTexSampler = this->renderDevice.createSampler(samplerDesc);
}

void pps::CopyPass::resize(glm::uvec2 size)
{
    this->size = size;
}

void pps::CopyPass::execute(std::map<Input, core::gl::Texture2D>&, core::gl::Texture2D prev,
                            core::gl::Framebuffer out) const
{
    // Set the framebuffer and state.
    this->renderDevice.setFramebuffer(out);
    this->renderDevice.setViewport(0, 0, static_cast<int>(this->size.x), static_cast<int>(this->size.y));
    this->renderDevice.setRasterState(nullptr);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setDepthStencilState(nullptr);

    // Set the pipeline and draw the screen quad.
    this->renderDevice.setShaderPipeline(this->pipeline);
    this->inputTexBP->bind(prev);
    this->inputTexBP->bind(inputTexSampler);
    this->renderDevice.setVertexArray(this->screenQuadVA);
    this->renderDevice.drawTriangles(0, 6);
}
