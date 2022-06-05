#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/gl/pps/bloom.hpp>

using namespace cubos;
using namespace cubos::core::gl;
using namespace cubos::engine::gl;

/// The vertex shader of the extraction step.
static const char* EXTRACTION_VS = R"glsl(
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

/// The pixel shader of the extraction step.
static const char* EXTRACTION_PS = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D inputTex;
uniform float brightnessThreshold;

out vec4 color;

#define LUMINANCE vec3(0.2126, 0.7152, 0.0722)

void main()
{
    vec4 texel = texture(inputTex, fragUv);
    float pixelIntensity = dot(texel.rgb, LUMINANCE);
    if (pixelIntensity < brightnessThreshold)
        discard;
    color = texel;
}
)glsl";

/// The vertex shader of the blur step.
static const char* BLUR_VS = R"glsl(
)glsl";

/// The pixel shader of the blur step.
static const char* BLUR_PS = R"glsl(
)glsl";

/// The vertex shader of the combine step.
static const char* COMBINE_VS = R"glsl(
)glsl";

/// The pixel shader of the combine step.
static const char* COMBINE_PS = R"glsl(
)glsl";

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size) : BloomPass(renderDevice, size, 1.0f, 1.0f) {}

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size, float brightnessThreshold, float lightBleedStrength) : Pass(renderDevice), size(size),
    brightnessThreshold(brightnessThreshold), lightBleedStrength(lightBleedStrength)
{
    // Create required textures
    // TODO:

    // Create the shader pipelines.
    /// Extraction
    auto vs = this->renderDevice.createShaderStage(Stage::Vertex, EXTRACTION_VS);
    auto ps = this->renderDevice.createShaderStage(Stage::Pixel, EXTRACTION_PS);
    this->extPipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->extInputTexBP = this->extPipeline->getBindingPoint("inputTex");
    this->extBrightnessThresholdBP = this->extPipeline->getBindingPoint("brightnessThreshold");

    /// Blur
    // TODO:

    /// Combine
    // TODO:

    // Create the screen quad VA.
    generateScreenQuad(this->renderDevice, this->extPipeline, this->screenQuadVA);

    // Create the samplers.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    this->texSampler1 = this->renderDevice.createSampler(samplerDesc);
    this->texSampler2 = this->renderDevice.createSampler(samplerDesc);
}

void pps::BloomPass::resize(glm::uvec2 size)
{
    this->size = size;
}

void pps::BloomPass::execute(std::map<Input, Texture2D>& inputs, Texture2D prev,
                            Framebuffer out) const
{
    // Set the framebuffer and state.
    this->renderDevice.setFramebuffer(out);
    this->renderDevice.setViewport(0, 0, this->size.x, this->size.y);
    this->renderDevice.setRasterState(nullptr);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setDepthStencilState(nullptr);
    this->renderDevice.setVertexArray(this->screenQuadVA);

    // Set the extraction pipeline and extract bright areas
    this->renderDevice.setShaderPipeline(this->extPipeline);
    this->extInputTexBP->bind(prev);
    this->extInputTexBP->bind(texSampler1);
    this->extBrightnessThresholdBP->setConstant(this->brightnessThreshold);
    this->renderDevice.clearTargetColor(0, 0, 0, 0, 1);
    this->renderDevice.drawTriangles(0, 6);

    // Set the blur pipeline to blur bright areas
    // TODO:

    // Set the combine pipeline to produce the final result
    // TODO:
}
