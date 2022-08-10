#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/gl/pps/bloom.hpp>

#include <math.h>

using namespace cubos;
using namespace cubos::core::gl;
using namespace cubos::engine::gl;

#define CUBOS_CORE_GL_PPS_BLOOM_DOWNSCALE_PASS 0
#define CUBOS_CORE_GL_PPS_BLOOM_UPSCALE_PASS 1
#define CUBOS_CORE_GL_PPS_BLOOM_COMBINE_PASS 2

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

#define LUMINANCE vec3(0.2126, 0.7152, 0.0722)

in vec2 fragUv;

uniform sampler2D inputTex;
uniform vec4 thresholdFilter;

out vec4 color;

void main()
{
    vec4 texel = texture(inputTex, fragUv);
    float pixelIntensity = dot(texel.rgb, LUMINANCE);
    float soft = pixelIntensity - thresholdFilter.y;
    soft = clamp(soft, 0.0, thresholdFilter.z);
    soft = soft * soft * thresholdFilter.w;
    float contrib = max(soft, pixelIntensity - thresholdFilter.x) / max(pixelIntensity, 0.00001);
    color = vec4(texel.rgb * contrib, texel.a);
}
)glsl";

/// The pixel shaders of the full bloom step.
/// This combines various sub-shaders such as downscale as upscale
/// to avoid code duplication and for better performance.
///
/// Source: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom
static const char* BLOOM_PS = R"glsl(
#version 330 core

#define DOWNSCALE_PASS 0
#define UPSCALE_PASS 1
#define COMBINE_PASS 2

in vec2 fragUv;

uniform sampler2D inputTex;
uniform sampler2D srcTex;
uniform float scaling;
uniform int currPass;
uniform float intensity;

out vec4 fragColor;

vec3 sample(vec2 uv)
{
    return texture(inputTex, uv).rgb;
}

vec4 sampleBox(vec2 texelSize, vec2 uv, float delta) {
    vec4 offset = texelSize.xyxy * vec4(-delta, -delta, delta, delta);
    vec3 color = sample(uv + offset.xy) + sample(uv + offset.zy) +
                 sample(uv + offset.xw) + sample(uv + offset.zw);
    return vec4(color * 0.25, 1);
}

void main(void)
{
    vec2 texelSize = 1.0 / textureSize(inputTex, 0);
    switch(currPass) {
        case DOWNSCALE_PASS:
            fragColor = sampleBox(texelSize, fragUv * scaling, 1.0);
            break;
        case UPSCALE_PASS:
            fragColor = sampleBox(texelSize, fragUv * scaling, 0.5);
            break;
        case COMBINE_PASS:
            fragColor = texture(srcTex, fragUv);
            fragColor += intensity * sampleBox(texelSize, fragUv * scaling, 0.5);
            fragColor.a = 1;
            break;
    }
}
)glsl";

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size)
    : BloomPass(renderDevice, size, 5, 1.0f, 0.5f, 1.0f)
{
}

pps::BloomPass::BloomPass(RenderDevice& renderDevice, glm::uvec2 size, unsigned int iterations, float threshold,
                          float softThreshold, float intensity)
    : Pass(renderDevice), size(size), iterations(iterations), threshold(threshold), softThreshold(softThreshold),
      intensity(intensity)
{
    generateTextures();

    // Create the shader pipelines.
    /// Extraction
    auto vs = this->renderDevice.createShaderStage(Stage::Vertex, COMMON_VS);
    auto ps = this->renderDevice.createShaderStage(Stage::Pixel, EXTRACTION_PS);
    this->extPipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->extInputTexBP = this->extPipeline->getBindingPoint("inputTex");
    this->extThresholdFilterBP = this->extPipeline->getBindingPoint("thresholdFilter");

    /// Dowscale
    vs = this->renderDevice.createShaderStage(Stage::Vertex, COMMON_VS);
    ps = this->renderDevice.createShaderStage(Stage::Pixel, BLOOM_PS);
    this->bloomPipeline = this->renderDevice.createShaderPipeline(vs, ps);
    this->bloomInputTexBP = this->bloomPipeline->getBindingPoint("inputTex");
    this->bloomSrcTexBP = this->bloomPipeline->getBindingPoint("srcTex");
    this->bloomScalingBP = this->bloomPipeline->getBindingPoint("scaling");
    this->bloomCurrPassBP = this->bloomPipeline->getBindingPoint("currPass");
    this->bloomIntensityBP = this->bloomPipeline->getBindingPoint("intensity");

    // Create the screen quad VA.
    generateScreenQuad(this->renderDevice, this->bloomPipeline, this->screenQuadVA);

    // Create the samplers.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Linear;
    samplerDesc.minFilter = TextureFilter::Linear;
    this->texSampler = this->renderDevice.createSampler(samplerDesc);

    // Create the blending settings
    BlendStateDesc blendDesc;
    blendDesc.blendEnabled = true;
    blendDesc.color.src = BlendFactor::One;
    blendDesc.color.dst = BlendFactor::One;
    blendDesc.alpha.src = BlendFactor::One;
    this->blendState = this->renderDevice.createBlendState(blendDesc);
}

float pps::BloomPass::getThreshold() const
{
    return threshold;
}

float pps::BloomPass::getSoftThreshold() const
{
    return softThreshold;
}

float pps::BloomPass::getIntensity() const
{
    return intensity;
}

void pps::BloomPass::setThreshold(float threshold)
{
    this->threshold = threshold;
}

void pps::BloomPass::setSoftThreshold(float softThreshold)
{
    this->softThreshold = softThreshold;
}

void pps::BloomPass::setIntensity(float intensity)
{
    this->intensity = intensity;
}

void pps::BloomPass::generateTextures()
{
    // Generate textures
    Texture2DDesc texDesc;
    texDesc.width = size.x;
    texDesc.height = size.y;
    texDesc.usage = Usage::Dynamic;
    texDesc.format = TextureFormat::RGBA32Float;
    extTex = renderDevice.createTexture2D(texDesc);

    bloomTexBuffer.clear();
    bloomTexBuffer.resize(iterations);
    for (int i = 0; i < iterations; i++)
    {
        texDesc.width /= 2;
        texDesc.height /= 2;
        if (texDesc.width == 0 or texDesc.height == 0)
        {
            iterations = i;
            bloomTexBuffer.resize(iterations);
            break;
        }
        bloomTexBuffer[i] = renderDevice.createTexture2D(texDesc);
    }

    // Generate framebuffers
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(extTex);
    extFB = renderDevice.createFramebuffer(fbDesc);

    bloomFBs.clear();
    bloomFBs.resize(iterations);
    fbDesc.targetCount = 1;
    for (int i = 0; i < iterations; i++)
    {
        fbDesc.targets[0].setTexture2DTarget(bloomTexBuffer[i]);
        bloomFBs[i] = renderDevice.createFramebuffer(fbDesc);
    }
}

void pps::BloomPass::resize(glm::uvec2 size)
{
    this->size = size;
    generateTextures();
}

void pps::BloomPass::execute(std::map<Input, Texture2D>& inputs, Texture2D prev, Framebuffer out) const
{
    // Set the framebuffer and state.
    this->renderDevice.setViewport(0, 0, this->size.x, this->size.y);
    this->renderDevice.setRasterState(nullptr);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setDepthStencilState(nullptr);
    this->renderDevice.setVertexArray(this->screenQuadVA);

    // Calculate all needed threshold values and submit packed as a vec4
    // to the shader for optimization.
    float knee = threshold * softThreshold;
    glm::vec4 filter;
    filter.x = threshold;
    filter.y = filter.x - knee;
    filter.z = 2 * knee;
    filter.w = 0.25f / (knee + 0.00001f);

    // Extraction pipeline, used to extract bright areas
    this->renderDevice.setFramebuffer(extFB);
    this->renderDevice.setShaderPipeline(this->extPipeline);
    this->extInputTexBP->bind(prev);
    this->extInputTexBP->bind(texSampler);
    this->extThresholdFilterBP->setConstant(filter);
    this->renderDevice.clearColor(0, 0, 0, 1);
    this->renderDevice.drawTriangles(0, 6);

    // Bloom pipeline, contains multiple operations combined for creating the final effect.
    this->renderDevice.setShaderPipeline(this->bloomPipeline);
    this->bloomInputTexBP->bind(extTex);
    this->bloomInputTexBP->bind(texSampler);

    // Downscale textures
    this->bloomCurrPassBP->setConstant(CUBOS_CORE_GL_PPS_BLOOM_DOWNSCALE_PASS);
    float scaling = 2.0f;
    for (int i = 0; i < iterations; i++)
    {
        this->bloomScalingBP->setConstant(scaling);
        this->renderDevice.setFramebuffer(bloomFBs[i]);
        this->renderDevice.drawTriangles(0, 6);
        this->bloomInputTexBP->bind(bloomTexBuffer[i]);
        scaling *= 2;
    }

    // Upscale textures additively
    scaling /= 2;
    this->bloomCurrPassBP->setConstant(CUBOS_CORE_GL_PPS_BLOOM_UPSCALE_PASS);
    this->renderDevice.setBlendState(this->blendState);
    for (int i = iterations - 2; i >= 0; i--)
    {
        scaling /= 2;
        this->bloomScalingBP->setConstant(scaling);
        this->renderDevice.setFramebuffer(bloomFBs[i]);
        this->renderDevice.drawTriangles(0, 6);
        this->bloomInputTexBP->bind(bloomTexBuffer[i]);
    }

    // Combine the final bloom effect with source texture
    scaling /= 2;
    float linIntensity = pow(this->intensity, 1 / 2.2f); // Converts from gamma to linear space

    this->bloomScalingBP->setConstant(scaling);
    this->bloomCurrPassBP->setConstant(CUBOS_CORE_GL_PPS_BLOOM_COMBINE_PASS);
    this->bloomIntensityBP->setConstant(linIntensity);
    this->bloomSrcTexBP->bind(prev);
    this->bloomSrcTexBP->bind(texSampler);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setFramebuffer(out);
    this->renderDevice.drawTriangles(0, 6);
}
