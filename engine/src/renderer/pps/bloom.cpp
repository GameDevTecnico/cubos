#include <cmath>

#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/renderer/pps/bloom.hpp>

using namespace cubos::core::gl;
using cubos::engine::PostProcessingBloom;

#define CUBOS_CORE_GL_PPS_BLOOM_DOWNSCALE_PASS 0
#define CUBOS_CORE_GL_PPS_BLOOM_UPSCALE_PASS 1
#define CUBOS_CORE_GL_PPS_BLOOM_COMBINE_PASS 2

/// The vertex shader of all steps.
static const char* commonVs = R"glsl(
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
static const char* extractionPs = R"glsl(
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
static const char* bloomPs = R"glsl(
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

PostProcessingBloom::PostProcessingBloom(RenderDevice& renderDevice, glm::uvec2 size)
    : PostProcessingBloom(renderDevice, size, 5, 1.0F, 0.5F, 1.0F)
{
}

PostProcessingBloom::PostProcessingBloom(RenderDevice& renderDevice, glm::uvec2 size, unsigned int iterations,
                                         float threshold, float softThreshold, float intensity)
    : PostProcessingPass(renderDevice)
    , mIterations(iterations)
    , mThreshold(threshold)
    , mSoftThreshold(softThreshold)
    , mIntensity(intensity)
    , mSize(size)
{
    generateTextures();

    // Create the shader pipelines.
    /// Extraction
    auto vs = mRenderDevice.createShaderStage(Stage::Vertex, commonVs);
    auto ps = mRenderDevice.createShaderStage(Stage::Pixel, extractionPs);
    mExtPipeline = mRenderDevice.createShaderPipeline(vs, ps);
    mExtInputTexBp = mExtPipeline->getBindingPoint("inputTex");
    mExtThresholdFilterBp = mExtPipeline->getBindingPoint("thresholdFilter");

    /// Dowscale
    vs = mRenderDevice.createShaderStage(Stage::Vertex, commonVs);
    ps = mRenderDevice.createShaderStage(Stage::Pixel, bloomPs);
    mBloomPipeline = mRenderDevice.createShaderPipeline(vs, ps);
    mBloomInputTexBp = mBloomPipeline->getBindingPoint("inputTex");
    mBloomSrcTexBp = mBloomPipeline->getBindingPoint("srcTex");
    mBloomScalingBp = mBloomPipeline->getBindingPoint("scaling");
    mBloomCurrPassBp = mBloomPipeline->getBindingPoint("currPass");
    mBloomIntensityBp = mBloomPipeline->getBindingPoint("intensity");

    // Create the screen quad VA.
    generateScreenQuad(mRenderDevice, mBloomPipeline, mScreenQuadVa);

    // Create the samplers.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Linear;
    samplerDesc.minFilter = TextureFilter::Linear;
    mTexSampler = mRenderDevice.createSampler(samplerDesc);

    // Create the blending settings
    BlendStateDesc blendDesc;
    blendDesc.blendEnabled = true;
    blendDesc.color.src = BlendFactor::One;
    blendDesc.color.dst = BlendFactor::One;
    blendDesc.alpha.src = BlendFactor::One;
    mBlendState = mRenderDevice.createBlendState(blendDesc);
}

float PostProcessingBloom::getThreshold() const
{
    return mThreshold;
}

float PostProcessingBloom::getSoftThreshold() const
{
    return mSoftThreshold;
}

float PostProcessingBloom::getIntensity() const
{
    return mIntensity;
}

void PostProcessingBloom::setThreshold(float threshold)
{
    mThreshold = threshold;
}

void PostProcessingBloom::setSoftThreshold(float softThreshold)
{
    mSoftThreshold = softThreshold;
}

void PostProcessingBloom::setIntensity(float intensity)
{
    mIntensity = intensity;
}

void PostProcessingBloom::generateTextures()
{
    // Generate textures
    Texture2DDesc texDesc;
    texDesc.width = mSize.x;
    texDesc.height = mSize.y;
    texDesc.usage = Usage::Dynamic;
    texDesc.format = TextureFormat::RGBA32Float;
    mExtTex = mRenderDevice.createTexture2D(texDesc);

    mBloomTexBuffer.clear();
    mBloomTexBuffer.resize(mIterations);
    for (unsigned int i = 0; i < mIterations; i++)
    {
        texDesc.width /= 2;
        texDesc.height /= 2;
        if (texDesc.width == 0 or texDesc.height == 0)
        {
            mIterations = i;
            mBloomTexBuffer.resize(mIterations);
            break;
        }
        mBloomTexBuffer[i] = mRenderDevice.createTexture2D(texDesc);
    }

    // Generate framebuffers
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(mExtTex);
    mExtFb = mRenderDevice.createFramebuffer(fbDesc);

    mBloomFBs.clear();
    mBloomFBs.resize(mIterations);
    fbDesc.targetCount = 1;
    for (unsigned int i = 0; i < mIterations; i++)
    {
        fbDesc.targets[0].setTexture2DTarget(mBloomTexBuffer[i]);
        mBloomFBs[i] = mRenderDevice.createFramebuffer(fbDesc);
    }
}

void PostProcessingBloom::resize(glm::uvec2 size)
{
    mSize = size;
    generateTextures();
}

void PostProcessingBloom::execute(std::map<PostProcessingInput, Texture2D>& /*inputs*/, Texture2D prev,
                                  Framebuffer out) const
{
    // Set the framebuffer and state.
    mRenderDevice.setViewport(0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y));
    mRenderDevice.setRasterState(nullptr);
    mRenderDevice.setBlendState(nullptr);
    mRenderDevice.setDepthStencilState(nullptr);
    mRenderDevice.setVertexArray(mScreenQuadVa);

    // Calculate all needed threshold values and submit packed as a vec4
    // to the shader for optimization.
    float knee = mThreshold * mSoftThreshold;
    glm::vec4 filter;
    filter.x = mThreshold;
    filter.y = filter.x - knee;
    filter.z = 2 * knee;
    filter.w = 0.25F / (knee + 0.00001F);

    // Extraction pipeline, used to extract bright areas
    mRenderDevice.setFramebuffer(mExtFb);
    mRenderDevice.setShaderPipeline(mExtPipeline);
    mExtInputTexBp->bind(prev);
    mExtInputTexBp->bind(mTexSampler);
    mExtThresholdFilterBp->setConstant(filter);
    mRenderDevice.clearColor(0, 0, 0, 1);
    mRenderDevice.drawTriangles(0, 6);

    // Bloom pipeline, contains multiple operations combined for creating the final effect.
    mRenderDevice.setShaderPipeline(mBloomPipeline);
    mBloomInputTexBp->bind(mExtTex);
    mBloomInputTexBp->bind(mTexSampler);

    // Downscale textures
    mBloomCurrPassBp->setConstant(CUBOS_CORE_GL_PPS_BLOOM_DOWNSCALE_PASS);
    float scaling = 2.0F;
    for (unsigned int i = 0; i < mIterations; i++)
    {
        mBloomScalingBp->setConstant(scaling);
        mRenderDevice.setFramebuffer(mBloomFBs[i]);
        mRenderDevice.drawTriangles(0, 6);
        mBloomInputTexBp->bind(mBloomTexBuffer[i]);
        scaling *= 2.0F;
    }

    // Upscale textures additively
    scaling /= 2.0F;
    mBloomCurrPassBp->setConstant(CUBOS_CORE_GL_PPS_BLOOM_UPSCALE_PASS);
    mRenderDevice.setBlendState(mBlendState);
    for (int i = static_cast<int>(mIterations) - 2; i >= 0; i--)
    {
        scaling /= 2.0F;
        mBloomScalingBp->setConstant(scaling);
        mRenderDevice.setFramebuffer(mBloomFBs[static_cast<std::size_t>(i)]);
        mRenderDevice.drawTriangles(0, 6);
        mBloomInputTexBp->bind(mBloomTexBuffer[static_cast<std::size_t>(i)]);
    }

    // Combine the final bloom effect with source texture
    scaling /= 2.0F;
    float linIntensity = powf(mIntensity, 1.0F / 2.2F); // Converts from gamma to linear space

    mBloomScalingBp->setConstant(scaling);
    mBloomCurrPassBp->setConstant(CUBOS_CORE_GL_PPS_BLOOM_COMBINE_PASS);
    mBloomIntensityBp->setConstant(linIntensity);
    mBloomSrcTexBp->bind(prev);
    mBloomSrcTexBp->bind(mTexSampler);
    mRenderDevice.setBlendState(nullptr);
    mRenderDevice.setFramebuffer(out);
    mRenderDevice.drawTriangles(0, 6);
}
