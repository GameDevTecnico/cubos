#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/gl/pps/bloom.hpp>

using namespace cubos;
using namespace cubos::core::gl;
using namespace cubos::engine::gl;

/// The vertex shader of all steps.
static const char* COMMON_VS = R"glsl(
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

/// The pixel shader of the blur step.
static const char* BLUR_PS = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D inputTex;
uniform float lightBleed;
uniform bool vertical;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 color;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(inputTex, 0); // gets size of single texel
    vec4 current_tex = texture(inputTex, fragUv);
    vec3 result = texture(inputTex, fragUv).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; i++) {
        if(vertical) {
            result += texture(inputTex, fragUv + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * lightBleed;
            result += texture(inputTex, fragUv - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * lightBleed;
        } else {
            result += texture(inputTex, fragUv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * lightBleed;
            result += texture(inputTex, fragUv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * lightBleed;
        }
    }
    color = vec4(result, 1.0);
}
)glsl";

/// The pixel shader of the combine step.
static const char* COMBINE_PS = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D renderTex;
uniform sampler2D blurTex;

out vec4 color;

void main()
{
    color = texture(renderTex, fragUv) + texture(blurTex, fragUv);
}
)glsl";

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size) : BloomPass(renderDevice, size, 0.8f, 1.0f) {}

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size, float brightnessThreshold, float lightBleedStrength) : Pass(renderDevice), size(size),
    brightnessThreshold(brightnessThreshold), lightBleedStrength(lightBleedStrength)
{
    // Create required textures
    Texture2DDesc texDesc;
    texDesc.width = size.x;
    texDesc.height = size.y;
    texDesc.usage = Usage::Dynamic;
    texDesc.format = TextureFormat::RGBA8UNorm;
    extTex = renderDevice.createTexture2D(texDesc);
    blurTex = renderDevice.createTexture2D(texDesc);

    // Create the shader pipelines.
    /// Extraction
    auto vs = this->renderDevice.createShaderStage(Stage::Vertex, COMMON_VS);
    auto ps = this->renderDevice.createShaderStage(Stage::Pixel, EXTRACTION_PS);
    this->extPipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->extInputTexBP = this->extPipeline->getBindingPoint("inputTex");
    this->extBrightnessThresholdBP = this->extPipeline->getBindingPoint("brightnessThreshold");
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(extTex);
    extFB = renderDevice.createFramebuffer(fbDesc);


    /// Blur
    ps = this->renderDevice.createShaderStage(Stage::Pixel, BLUR_PS);
    this->blurPipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->blurInputTexBP = this->blurPipeline->getBindingPoint("inputTex");
    this->blurLightBleedBP = this->blurPipeline->getBindingPoint("lightBleed");
    this->blurVerticalBP = this->blurPipeline->getBindingPoint("vertical");
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(blurTex);
    blurFB = renderDevice.createFramebuffer(fbDesc);

    /// Combine
    ps = this->renderDevice.createShaderStage(Stage::Pixel, COMBINE_PS);
    this->combinePipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->combineRenderTexBP = this->combinePipeline->getBindingPoint("renderTex");
    this->combineBlurTexBP = this->combinePipeline->getBindingPoint("blurTex");

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
    this->renderDevice.setViewport(0, 0, this->size.x, this->size.y);
    this->renderDevice.setRasterState(nullptr);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setDepthStencilState(nullptr);
    this->renderDevice.setVertexArray(this->screenQuadVA);

    // Set the extraction pipeline and extract bright areas
    this->renderDevice.setFramebuffer(extFB);
    this->renderDevice.setShaderPipeline(this->extPipeline);
    this->extInputTexBP->bind(prev);
    this->extInputTexBP->bind(texSampler1);
    this->extBrightnessThresholdBP->setConstant(this->brightnessThreshold);
    this->renderDevice.clearColor(0, 0, 0, 1);
    this->renderDevice.drawTriangles(0, 6);

    // Set the blur pipeline to blur bright areas
    this->renderDevice.setFramebuffer(blurFB);
    this->renderDevice.setShaderPipeline(blurPipeline);
    this->blurInputTexBP->bind(extTex);
    this->blurInputTexBP->bind(texSampler1);
    this->blurLightBleedBP->setConstant(lightBleedStrength);
    this->renderDevice.clearColor(0, 0, 0, 1);
    for(int i = 0; i < 5; i++) {
        this->blurVerticalBP->setConstant(false);
        this->renderDevice.drawTriangles(0, 6);
        // The blur step starts with the texture from extraction step, but from now on
        // it needs to work on previous iterations. Bind it at first time this happens.
        if (i == 0)
            this->blurInputTexBP->bind(blurTex);
        this->blurVerticalBP->setConstant(true);
        this->renderDevice.drawTriangles(0, 6);
    }

    // Set the combine pipeline to produce the final result
    this->renderDevice.setFramebuffer(out);
    this->renderDevice.setShaderPipeline(combinePipeline);
    this->combineRenderTexBP->bind(prev);
    this->combineRenderTexBP->bind(texSampler1);
    this->combineBlurTexBP->bind(blurTex);
    this->combineBlurTexBP->bind(texSampler2);
    this->renderDevice.clearColor(0, 0, 0, 1);
    this->renderDevice.drawTriangles(0, 6);
}
