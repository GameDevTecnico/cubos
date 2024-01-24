#include <cstring>
#include <random>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cubos/core/gl/debug.hpp>
#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/renderer/deferred_renderer.hpp>
#include <cubos/engine/renderer/frame.hpp>
#include <cubos/engine/renderer/vertex.hpp>

using namespace cubos::core::gl;
using cubos::engine::DeferredRenderer;

/// Deferred renderer grid implementation.
struct DeferredGrid : public cubos::engine::impl::RendererGrid
{
    VertexArray va;
    IndexBuffer ib;
    std::size_t indexCount;
};

/// Holds the model view matrix sent to the GPU.
struct MVP
{
    glm::mat4 m;
    glm::mat4 v;
    glm::mat4 p;
};

// Holds the data of a spot light, ready to be sent to the lighting pass pipeline.
struct SpotLightData
{
    glm::vec4 position;
    glm::mat4 rotation;
    glm::vec4 color;
    float intensity;
    float range;
    float spotCutoff;
    float innerSpotCutoff;
};

// Holds the data of a directional light, ready to be sent to the lighting pass pipeline.
struct DirectionalLightData
{
    glm::mat4 rotation;
    glm::vec4 color;
    float intensity;
    float padding[3]; // Necessary to align the struct to a 16 byte boundary.
};

// Holds the data of a point light, ready to be sent to the lighting pass pipeline.
struct PointLightData
{
    glm::vec4 position;
    glm::vec4 color;
    float intensity;
    float range;
    float padding[2]; // Necessary to align the struct to a 16 byte boundary.
};

/// Holds all of the light data sent to the lighting pass pipeline.
struct LightsData
{
    glm::vec4 ambientLight;
    SpotLightData spotLights[CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT];
    DirectionalLightData directionalLights[CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT];
    PointLightData pointLights[CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT];
    uint32_t numSpotLights;
    uint32_t numDirectionalLights;
    uint32_t numPointLights;
};

/// The vertex shader of the geometry pass pipeline.
static const char* geometryPassVs = R"glsl(
#version 330 core

in uvec3 position;
in vec3 normal;
in uint material;

out vec3 fragPosition;
out vec3 fragNormal;
flat out uint fragMaterial;

uniform MVP
{
    mat4 M;
    mat4 V;
    mat4 P;
};

void main()
{
    vec4 worldPosition = M * vec4(position, 1.0);
    vec4 viewPosition = V * worldPosition;
    fragPosition = vec3(worldPosition);

    mat3 N = transpose(inverse(mat3(M)));
    fragNormal = N * normal;

    gl_Position = P * viewPosition;

    fragMaterial = material;
}
)glsl";

/// The pixel shader of the geometry pass pipeline.
static const char* geometryPassPs = R"glsl(
#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
flat in uint fragMaterial;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out uint material;

void main()
{
    position = fragPosition;
    normal = normalize(fragNormal);
    material = fragMaterial;
}
)glsl";

/// The vertex shader of the screen picking pass pipeline.
static const char* pickingPassVs = R"glsl(
#version 330 core

in uvec3 position;
in vec3 normal;
in uint material;

uniform MVP
{
    mat4 M;
    mat4 V;
    mat4 P;
};

void main()
{
    vec4 worldPosition = M * vec4(position, 1.0);
    vec4 viewPosition = V * worldPosition;

    gl_Position = P * viewPosition;
}
)glsl";

// The pixel shader of the screen picking pass pipeline.
static const char* pickingPassPs = R"glsl(
#version 330 core

uniform uint entityIndex;

out uvec2 idOutput;

void main()
{
    idOutput.r = (entityIndex >> 16U);
    idOutput.g = (entityIndex & 65535U);
}
)glsl";

/// The vertex shader of the lighting pass pipeline.
static const char* lightingPassVs = R"glsl(
#version 330 core

in vec4 position;
in vec2 uv;

out vec2 fragUv;

uniform vec2 uvScale;
uniform vec2 uvOffset;

void main(void)
{
    gl_Position = position;
    fragUv = uv * uvScale + uvOffset;
}
)glsl";

/// The pixel shader of the lighting pass pipeline.
static const char* lightingPassPs = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D palette;
uniform sampler2D position;
uniform sampler2D normal;
uniform usampler2D material;

uniform bool ssaoEnabled;
uniform sampler2D ssaoTex;

uniform vec3 skyGradient[2];
uniform vec2 uvScale;
uniform vec2 uvOffset;
uniform mat4 invV;
uniform mat4 invP;

struct SpotLight
{
    vec4 position;
    mat4 rotation;
    vec4 color;
    float intensity;
    float range;
    float spotCutoff;
    float innerSpotCutoff;
};

struct DirectionalLight
{
    mat4 rotation;
    vec4 color;
    float intensity;
};

struct PointLight
{
    vec4 position;
    vec4 color;
    float intensity;
    float range;
};

layout(std140) uniform Lights
{
    vec4 ambientLight;
    SpotLight spotLights[128];
    DirectionalLight directionalLights[128];
    PointLight pointLights[128];
    uint numSpotLights;
    uint numDirectionalLights;
    uint numPointLights;
};

layout(location = 0) out vec4 color;

float remap(float value, float min1, float max1, float min2, float max2) {
    return max2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 spotLightCalc(vec3 fragPos, vec3 fragNormal, SpotLight light) {
    vec3 toLight = vec3(light.position) - fragPos;
    float r = length(toLight) / light.range;
    if (r < 1) {
        vec3 toLightNormalized = normalize(toLight);
        float a = dot(toLightNormalized, -vec3(light.rotation * vec4(0,0,1,1)));
        if (a > light.spotCutoff) {
            float angleValue = clamp(remap(a, light.innerSpotCutoff, light.spotCutoff, 1, 0), 0, 1);
            float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0 , 1);
            float diffuse = max(dot(fragNormal, toLightNormalized), 0);
            return angleValue * attenuation * diffuse * light.intensity * vec3(light.color);
        }
    }
    return vec3(0);
}

vec3 directionalLightCalc(vec3 fragNormal, DirectionalLight light)
{
    return max(dot(fragNormal, -vec3(light.rotation * vec4(0,0,1,1))), 0) * light.intensity * vec3(light.color);
}

vec3 pointLightCalc(vec3 fragPos, vec3 fragNormal, PointLight light) {
    vec3 toLight = vec3(light.position) - fragPos;
    float r = length(toLight) / light.range;
    if (r < 1) {
        float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
        float diffuse = max(dot(fragNormal, vec3(normalize(toLight))), 0);
        return attenuation * diffuse * light.intensity * vec3(light.color);
    }
    return vec3(0);
}

vec4 fetchAlbedo(uint material)
{
    return texelFetch(palette, ivec2(mod(material, 256u), material / 256u), 0);
}

vec3 rayDir(vec2 uv)
{
    // Convert fragment coords to normalized device space.
    vec4 pointNDS = vec4(uv * 2.0 - 1.0, -1.0, 1.0);
    // Get the point in view space.
    vec3 pointView = (invP * pointNDS).xyz;
    // Get the direction in world space.
    vec3 dir = (invV * vec4(pointView, 0.0)).xyz;
    return normalize(dir);
}

void main()
{
    uint m = texture(material, fragUv).r;
    if (m == 0u) {
        vec3 dir = rayDir((fragUv - uvOffset) / uvScale);
        color = vec4(mix(skyGradient[0], skyGradient[1], clamp(dir.y * 0.5 + 0.5, 0.0, 1.0)), 1.0);
    } else {
        vec3 albedo = fetchAlbedo(m).rgb;
        vec3 lighting = ambientLight.rgb;
        vec3 fragPos = texture(position, fragUv).xyz;
        vec3 fragNormal = texture(normal, fragUv).xyz;
        for (uint i = 0u; i < numSpotLights; i++) {
            lighting += spotLightCalc(fragPos, fragNormal, spotLights[i]);
        }
        for (uint i = 0u; i < numDirectionalLights; i++) {
            lighting += directionalLightCalc(fragNormal, directionalLights[i]);
        }
        for (uint i = 0u; i < numPointLights; i++) {
            lighting += pointLightCalc(fragPos, fragNormal, pointLights[i]);
        }
        color = vec4(albedo * lighting, 1.0);
        color.r = min(color.r, 1.0);
        color.g = min(color.g, 1.0);
        color.b = min(color.b, 1.0);
        if (ssaoEnabled) {
            color *= texture(ssaoTex, fragUv).r;
        }
    }
}
)glsl";

/// The vertex shader of the SSAO pass pipeline.
static const char* ssaoPassVs = R"glsl(
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

/// The pixel shader of the SSAO pass pipeline.
static const char* ssaoPassPs = R"glsl(
#version 330 core

#define KERNEL_SIZE 64
#define RADIUS 0.5
#define BIAS 0.025

in vec2 fragUv;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D noise;
uniform vec3 samples[KERNEL_SIZE];
uniform mat4 view;
uniform mat4 projection;
uniform vec2 screenSize;


layout (location = 0) out float color;

vec3 getFragPos(vec2 fragUv) {
    vec4 tmp = texture(position, fragUv);
    return (view * tmp).xyz;
}

vec3 getFragNorm(vec2 fragUv) {
    vec4 tmp = texture(normal, fragUv);
    tmp.w = 0;
    return (view * tmp).xyz;
}

void main(void)
{
    vec3 fragPos = getFragPos(fragUv);
    vec3 fragNormal = getFragNorm(fragUv);
    vec2 noiseScale = screenSize / 4;
    vec3 randVec = normalize(texture(noise, fragUv * noiseScale).xyz);

    vec3 tangent = normalize(randVec - fragNormal * dot(randVec, fragNormal));
    vec3 bitangent = cross(fragNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, fragNormal);

    float occlusion = 0.0;
    for(int i = 0; i < KERNEL_SIZE; i++)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * RADIUS;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = getFragPos(offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    color = occlusion;
}
)glsl";

// The pixel shader of the SSAO blur pass pipeline.
static const char* ssaoBlurPs = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D ssaoInput;

layout (location = 0) out float color;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, fragUv + offset).r;
        }
    }
    color = result / (4.0 * 4.0);
}
)glsl";

DeferredRenderer::DeferredRenderer(RenderDevice& renderDevice, glm::uvec2 size, Settings& settings)
    : BaseRenderer(renderDevice, size)
{
    // Create the states.
    RasterStateDesc rasterStateDesc;
    rasterStateDesc.frontFace = Winding::CCW;
    rasterStateDesc.cullFace = Face::Back;
    rasterStateDesc.cullEnabled = true;
    mGeometryRasterState = mRenderDevice.createRasterState(rasterStateDesc);
    mPickingRasterState = mRenderDevice.createRasterState(rasterStateDesc);

    BlendStateDesc blendStateDesc;
    mGeometryBlendState = mRenderDevice.createBlendState(blendStateDesc);
    mPickingBlendState = mRenderDevice.createBlendState(blendStateDesc);

    DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = true;
    depthStencilStateDesc.depth.writeEnabled = true;
    mGeometryDepthStencilState = mRenderDevice.createDepthStencilState(depthStencilStateDesc);
    mPickingDepthStencilState = mRenderDevice.createDepthStencilState(depthStencilStateDesc);

    // Create the geometry pipeline.
    auto geometryVS = mRenderDevice.createShaderStage(Stage::Vertex, geometryPassVs);
    auto geometryPS = mRenderDevice.createShaderStage(Stage::Pixel, geometryPassPs);
    mGeometryPipeline = mRenderDevice.createShaderPipeline(geometryVS, geometryPS);
    mVpBp = mGeometryPipeline->getBindingPoint("MVP");

    // Create the MVP constant buffer.
    mVpBuffer = renderDevice.createConstantBuffer(sizeof(MVP), nullptr, Usage::Dynamic);

    // Create the screen picking pipeline.
    auto pickingVS = mRenderDevice.createShaderStage(Stage::Vertex, pickingPassVs);
    auto pickingPS = mRenderDevice.createShaderStage(Stage::Pixel, pickingPassPs);
    mPickingPipeline = mRenderDevice.createShaderPipeline(pickingVS, pickingPS);
    mPickingMVpBp = mPickingPipeline->getBindingPoint("MVP");
    mPickingIndexBp = mPickingPipeline->getBindingPoint("entityIndex");

    // Create the lighting pipeline.
    auto lightingVS = mRenderDevice.createShaderStage(Stage::Vertex, lightingPassVs);
    auto lightingPS = mRenderDevice.createShaderStage(Stage::Pixel, lightingPassPs);
    mLightingPipeline = mRenderDevice.createShaderPipeline(lightingVS, lightingPS);
    mPositionBp = mLightingPipeline->getBindingPoint("position");
    mNormalBp = mLightingPipeline->getBindingPoint("normal");
    mMaterialBp = mLightingPipeline->getBindingPoint("material");
    mPaletteBp = mLightingPipeline->getBindingPoint("palette");
    mLightsBp = mLightingPipeline->getBindingPoint("Lights");
    mSsaoEnabledBp = mLightingPipeline->getBindingPoint("ssaoEnabled");
    mSsaoTexBp = mLightingPipeline->getBindingPoint("ssaoTex");
    mUVScaleBp = mLightingPipeline->getBindingPoint("uvScale");
    mUVOffsetBp = mLightingPipeline->getBindingPoint("uvOffset");
    mSkyGradientBottomBp = mLightingPipeline->getBindingPoint("skyGradient[0]");
    mSkyGradientTopBp = mLightingPipeline->getBindingPoint("skyGradient[1]");
    mInvVBp = mLightingPipeline->getBindingPoint("invV");
    mInvPBp = mLightingPipeline->getBindingPoint("invP");

    // Create the SSAO pipeline.
    auto ssaoVS = mRenderDevice.createShaderStage(Stage::Vertex, ssaoPassVs);
    auto ssaoPS = mRenderDevice.createShaderStage(Stage::Pixel, ssaoPassPs);
    mSsaoPipeline = mRenderDevice.createShaderPipeline(ssaoVS, ssaoPS);
    mSsaoPositionBp = mSsaoPipeline->getBindingPoint("position");
    mSsaoNormalBp = mSsaoPipeline->getBindingPoint("normal");
    mSsaoNoiseBp = mSsaoPipeline->getBindingPoint("noise");
    mSsaoViewBp = mSsaoPipeline->getBindingPoint("view");
    mSsaoProjectionBp = mSsaoPipeline->getBindingPoint("projection");
    mSsaoScreenSizeBp = mSsaoPipeline->getBindingPoint("screenSize");

    // Create the SSAO blur pipeline.
    auto ssaoBlurPS = mRenderDevice.createShaderStage(Stage::Pixel, ssaoBlurPs);
    mSsaoBlurPipeline = mRenderDevice.createShaderPipeline(ssaoVS, ssaoBlurPS);
    mSsaoBlurTexBp = mSsaoBlurPipeline->getBindingPoint("ssaoInput");

    // Create the sampler used to access the palette and the GBuffer textures in the lighting pipeline.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    mSampler = mRenderDevice.createSampler(samplerDesc);

    // Create the sampler for the SSAO noise texture
    samplerDesc.addressU = samplerDesc.addressV = samplerDesc.addressW = AddressMode::Repeat;
    mSsaoNoiseSampler = mRenderDevice.createSampler(samplerDesc);

    // Create the palette texture.
    Texture2DDesc texDesc;
    texDesc.width = 256;
    texDesc.height = 256;
    texDesc.format = TextureFormat::RGBA32Float;
    texDesc.usage = Usage::Default;
    mPaletteTex = mRenderDevice.createTexture2D(texDesc);

    // Create the lights constant buffer.
    mLightsBuffer = mRenderDevice.createConstantBuffer(sizeof(LightsData), nullptr, Usage::Dynamic);

    // Generate a screen quad for the lighting pass.
    generateScreenQuad(mRenderDevice, mLightingPipeline, mScreenQuadVa);

    // Create the GBuffer.
    mSize = glm::uvec2(0, 0);
    DeferredRenderer::onResize(size);

    // Check whether SSAO is enabled.
    mSsaoEnabled = settings.getBool("renderer.ssao.enabled", false);
    if (mSsaoEnabled)
    {
        createSSAOTextures();
        generateSSAONoise();
    }

    /// FIXME: This should not be on production code.
    core::gl::Debug::init(mRenderDevice);
}

DeferredRenderer::~DeferredRenderer()
{
    /// FIXME: This should not be on production code.
    core::gl::Debug::terminate();
}

cubos::engine::RendererGrid DeferredRenderer::upload(const VoxelGrid& grid)
{
    auto deferredGrid = std::make_shared<DeferredGrid>();

    // First, triangulate the grid.
    // This may be improved in the future by doing it in a separate thread and only blocking on it when the grid needs
    // to be drawn.
    std::vector<VoxelVertex> vertices;
    std::vector<uint32_t> indices;
    triangulate(grid, vertices, indices);

    // Create the vertex array, vertex buffer and index buffer.
    VertexArrayDesc vaDesc;
    vaDesc.elementCount = 3;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = Type::UInt;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = offsetof(VoxelVertex, position);
    vaDesc.elements[0].buffer.stride = sizeof(VoxelVertex);
    vaDesc.elements[1].name = "normal";
    vaDesc.elements[1].type = Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 0;
    vaDesc.elements[1].buffer.offset = offsetof(VoxelVertex, normal);
    vaDesc.elements[1].buffer.stride = sizeof(VoxelVertex);
    vaDesc.elements[2].name = "material";
    vaDesc.elements[2].type = Type::UShort;
    vaDesc.elements[2].size = 1;
    vaDesc.elements[2].buffer.index = 0;
    vaDesc.elements[2].buffer.offset = offsetof(VoxelVertex, material);
    vaDesc.elements[2].buffer.stride = sizeof(VoxelVertex);
    vaDesc.buffers[0] =
        mRenderDevice.createVertexBuffer(vertices.size() * sizeof(VoxelVertex), vertices.data(), Usage::Static);
    vaDesc.shaderPipeline = mGeometryPipeline;
    deferredGrid->va = mRenderDevice.createVertexArray(vaDesc);
    deferredGrid->ib = mRenderDevice.createIndexBuffer(indices.size() * sizeof(uint32_t), indices.data(),
                                                       IndexFormat::UInt, Usage::Static);
    deferredGrid->indexCount = indices.size();

    return deferredGrid;
}

void DeferredRenderer::setPalette(const VoxelPalette& palette)
{
    // Get the colors from the palette.
    // Magenta is used for non-existent materials in order to easily identify errors.
    std::vector<glm::vec4> data(65536, {1.0F, 0.0F, 1.0F, 1.0F});
    for (std::size_t i = 0; i < palette.size(); ++i)
    {
        if (palette.data()[i].similarity(VoxelMaterial::Empty) < 1.0F)
        {
            data[i + 1] = palette.data()[i].color;
        }
    }

    // Update the data on the GPU.
    mPaletteTex->update(0, 0, 256, 256, data.data());
}

void DeferredRenderer::onResize(glm::uvec2 size)
{
    // Only resize if the size has changed.
    if (mSize == size)
    {
        return;
    }

    mSize = size;
    Texture2DDesc texDesc;
    texDesc.width = mSize.x;
    texDesc.height = mSize.y;
    texDesc.usage = Usage::Dynamic;

    // Create the position and normal textures.
    texDesc.format = TextureFormat::RGB32Float;
    mPositionTex = mRenderDevice.createTexture2D(texDesc);
    mNormalTex = mRenderDevice.createTexture2D(texDesc);

    // Create the material texture.
    texDesc.format = TextureFormat::R16UInt;
    mMaterialTex = mRenderDevice.createTexture2D(texDesc);

    // Create the depth texture.
    texDesc.format = TextureFormat::Depth24Stencil8;
    mDepthTex = mRenderDevice.createTexture2D(texDesc);

    // Create the framebuffer for the geometry pass.
    FramebufferDesc gBufferDesc;
    gBufferDesc.targetCount = 3;
    gBufferDesc.targets[0].setTexture2DTarget(mPositionTex);
    gBufferDesc.targets[1].setTexture2DTarget(mNormalTex);
    gBufferDesc.targets[2].setTexture2DTarget(mMaterialTex);
    gBufferDesc.depthStencil.setTexture2DTarget(mDepthTex);
    mGBuffer = mRenderDevice.createFramebuffer(gBufferDesc);

    if (mSsaoEnabled)
    {
        createSSAOTextures();
    }
}

void DeferredRenderer::onRender(const glm::mat4& view, const Viewport& viewport, const Camera& camera,
                                const RendererFrame& frame, Framebuffer target, core::gl::Framebuffer pickingBuffer,
                                bool enableScreenPicking)
{
    // Steps:
    // 1. Prepare the MVP matrix.
    // 2. Fill the light buffer with the light data.
    // 3. Set the renderer state.
    // 4. Geometry pass:
    //   1. Set the geometry pass state.
    //   2. Clear the GBuffer.
    //   3. For each draw command:
    //     1. Update the MVP constant buffer with the model matrix.
    //     2. Draw the geometry.
    // 5. Picking pass:
    //   1. Set the picking pass state.
    //   2. For each draw command:
    //     1. Update the MVP constant buffer with the model matrix.
    //     2. Draw the entity identifiers to the entity picking framebuffer.
    // 6. SSAO pass
    // 7. Lighting pass:
    //   1. Set the lighting pass state.
    //   2. Draw the screen quad.

    // 1. Prepare the MVP matrix.
    MVP mvp;
    mvp.v = view;
    mvp.p = glm::perspective(glm::radians(camera.fovY), float(viewport.size.x) / float(viewport.size.y), camera.zNear,
                             camera.zFar);

    // 2. Fill the light buffer with the light data.
    // First map the buffer.
    LightsData& lightData = *static_cast<LightsData*>(mLightsBuffer->map());
    lightData.numSpotLights = 0;
    lightData.numDirectionalLights = 0;
    lightData.numPointLights = 0;

    // Set the ambient light.
    lightData.ambientLight = glm::vec4(frame.ambient(), 1.0F);

    // Spotlights.
    for (const auto& [transform, light] : frame.spotLights())
    {
        if (lightData.numSpotLights >= CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT)
        {
            CUBOS_WARN("Number of spot lights to be drawn "
                       "this frame exceeds the maximum allowed ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT);
            break;
        }

        lightData.spotLights[lightData.numSpotLights].position = transform * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
        lightData.spotLights[lightData.numSpotLights].rotation = glm::toMat4(glm::quat_cast(transform));
        lightData.spotLights[lightData.numSpotLights].color = glm::vec4(light.color, 1.0F);
        lightData.spotLights[lightData.numSpotLights].intensity = light.intensity;
        lightData.spotLights[lightData.numSpotLights].range = light.range;
        lightData.spotLights[lightData.numSpotLights].spotCutoff = glm::cos(light.spotAngle);
        lightData.numSpotLights += 1;
    }

    // Directional lights.
    for (const auto& [transform, light] : frame.directionalLights())
    {
        if (lightData.numDirectionalLights >= CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT)
        {
            CUBOS_WARN("Number of directional lights to be drawn "
                       "this frame exceeds the maximum allowed ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT);
            break;
        }

        lightData.directionalLights[lightData.numDirectionalLights].rotation = glm::toMat4(glm::quat_cast(transform));
        lightData.directionalLights[lightData.numDirectionalLights].color = glm::vec4(light.color, 1.0F);
        lightData.directionalLights[lightData.numDirectionalLights].intensity = light.intensity;
        lightData.numDirectionalLights += 1;
    }

    // Point lights.
    for (const auto& [transform, light] : frame.pointLights())
    {
        if (lightData.numPointLights >= CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT)
        {
            CUBOS_WARN("Number of point lights to be drawn "
                       "this frame exceeds the maximum allowed ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT);
            break;
        }
        lightData.pointLights[lightData.numPointLights].position = transform * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
        lightData.pointLights[lightData.numPointLights].color = glm::vec4(light.color, 1.0F);
        lightData.pointLights[lightData.numPointLights].intensity = light.intensity;
        lightData.pointLights[lightData.numPointLights].range = light.range;
        lightData.numPointLights += 1;
    }

    // Unmap the buffer.
    mLightsBuffer->unmap();

    // 3. Set the renderer state.
    mRenderDevice.setViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

    // 4. Geometry pass.
    // 4.1. Set the geometry pass state.
    mRenderDevice.setFramebuffer(mGBuffer);
    mRenderDevice.setRasterState(mGeometryRasterState);
    mRenderDevice.setBlendState(mGeometryBlendState);
    mRenderDevice.setDepthStencilState(mGeometryDepthStencilState);
    mRenderDevice.setShaderPipeline(mGeometryPipeline);
    mVpBp->bind(mVpBuffer);

    // 4.2. Clear the GBuffer.
    mRenderDevice.clearTargetColor(0, 0.0F, 0.0F, 0.0F, 1.0F);
    mRenderDevice.clearTargetColor(1, 0.0F, 0.0F, 0.0F, 1.0F);
    mRenderDevice.clearTargetColor(2, 0.0F, 0.0F, 0.0F, 0.0F);
    mRenderDevice.clearDepth(1.0F);

    // 4.3. For each draw command:
    for (const auto& drawCmd : frame.drawCmds())
    {
        // 4.3.1. Update the MVP constant buffer with the model matrix.
        mvp.m = drawCmd.modelMat;
        memcpy(mVpBuffer->map(), &mvp, sizeof(MVP));
        mVpBuffer->unmap();

        // 4.3.2. Draw the geometry.
        auto grid = std::static_pointer_cast<DeferredGrid>(drawCmd.grid);
        mRenderDevice.setVertexArray(grid->va);
        mRenderDevice.setIndexBuffer(grid->ib);
        mRenderDevice.drawTrianglesIndexed(0, grid->indexCount);
    }

    if (enableScreenPicking)
    {
        // 5. Picking pass:
        // 5.1. Set the picking pass state.
        mRenderDevice.setFramebuffer(pickingBuffer);
        mRenderDevice.setRasterState(mPickingRasterState);
        mRenderDevice.setBlendState(mPickingBlendState);
        mRenderDevice.setDepthStencilState(mPickingDepthStencilState);
        mRenderDevice.setShaderPipeline(mPickingPipeline);
        mPickingMVpBp->bind(mVpBuffer);

        // 5.2. For each draw command:
        for (const auto& drawCmd : frame.drawCmds())
        {
            // 5.2.1. Update the MVP constant buffer with the model matrix.
            mvp.m = drawCmd.modelMat;
            memcpy(mVpBuffer->map(), &mvp, sizeof(MVP));
            mVpBuffer->unmap();

            // 5.2.2. Draw the entity identifiers to the entity picking framebuffer.
            auto grid = std::static_pointer_cast<DeferredGrid>(drawCmd.grid);
            mPickingIndexBp->setConstant(drawCmd.entityIndex);
            mRenderDevice.setVertexArray(grid->va);
            mRenderDevice.setIndexBuffer(grid->ib);
            mRenderDevice.drawTrianglesIndexed(0, grid->indexCount);
        }
    }

    // 6. SSAO pass.
    if (mSsaoEnabled)
    {
        // 6.1. Set the SSAO pass state.
        mRenderDevice.setFramebuffer(mSsaoFb);
        mRenderDevice.setRasterState(nullptr);
        mRenderDevice.setBlendState(nullptr);
        mRenderDevice.setDepthStencilState(nullptr);
        mRenderDevice.setShaderPipeline(mSsaoPipeline);
        mSsaoPositionBp->bind(mPositionTex);
        mSsaoPositionBp->bind(mSampler);
        mSsaoNormalBp->bind(mNormalTex);
        mSsaoNormalBp->bind(mSampler);
        mSsaoNoiseBp->bind(mSsaoNoiseTex);
        mSsaoNoiseBp->bind(mSsaoNoiseSampler);
        mSsaoViewBp->setConstant(mvp.v);
        mSsaoProjectionBp->setConstant(mvp.p);
        mSsaoScreenSizeBp->setConstant(glm::vec2(mSize));

        // Samples
        for (int i = 0; i < 64; i++)
        {
            mSsaoSamplesBp = mSsaoPipeline->getBindingPoint(std::string("samples[" + std::to_string(i) + "]").c_str());
            mSsaoSamplesBp->setConstant(mSsaoKernel[static_cast<std::size_t>(i)]);
        }

        mRenderDevice.setVertexArray(mScreenQuadVa);
        mRenderDevice.drawTriangles(0, 6);

        // 6.2. Set the SSAO blur pass state.
        mRenderDevice.setShaderPipeline(mSsaoBlurPipeline);
        mSsaoBlurTexBp->bind(mSsaoTex);
        mRenderDevice.drawTriangles(0, 6);
    }

    // 7. Lighting pass.
    // 7.1. Set the lighting pass state.
    mRenderDevice.setFramebuffer(target);
    mRenderDevice.setRasterState(nullptr);
    mRenderDevice.setBlendState(nullptr);
    mRenderDevice.setDepthStencilState(nullptr);
    mRenderDevice.setShaderPipeline(mLightingPipeline);
    mPositionBp->bind(mPositionTex);
    mPositionBp->bind(mSampler);
    mNormalBp->bind(mNormalTex);
    mNormalBp->bind(mSampler);
    mMaterialBp->bind(mMaterialTex);
    mMaterialBp->bind(mSampler);
    mPaletteBp->bind(mPaletteTex);
    mPaletteBp->bind(mSampler);
    mLightsBp->bind(mLightsBuffer);
    mSsaoEnabledBp->setConstant(static_cast<int>(mSsaoEnabled));
    if (mSsaoEnabledBp != nullptr)
    {
        mSsaoTexBp->bind(mSsaoTex);
        mSsaoTexBp->bind(mSampler);
    }
    mUVScaleBp->setConstant(
        glm::vec2((float)viewport.size.x / (float)mSize.x, (float)viewport.size.y / (float)mSize.y));
    mUVOffsetBp->setConstant(
        glm::vec2((float)viewport.position.x / (float)mSize.x, (float)viewport.position.y / (float)mSize.y));
    mSkyGradientBottomBp->setConstant(frame.skyGradient(0));
    mSkyGradientTopBp->setConstant(frame.skyGradient(1));
    mInvVBp->setConstant(glm::inverse(mvp.v));
    mInvPBp->setConstant(glm::inverse(mvp.p));

    // 7.3. Draw the screen quad.
    mRenderDevice.setVertexArray(mScreenQuadVa);
    mRenderDevice.drawTriangles(0, 6);

    /// FIXME: This should not be on production code.
    core::gl::Debug::flush(mvp.p * mvp.v, 1 / 60.0F);

    // Provide custom inputs to the PPS manager.
    this->pps().provideInput(PostProcessingInput::Position, mPositionTex);
    this->pps().provideInput(PostProcessingInput::Normal, mNormalTex);
}

void DeferredRenderer::createSSAOTextures()
{
    Texture2DDesc texDesc;
    texDesc.width = mSize.x;
    texDesc.height = mSize.y;
    texDesc.usage = Usage::Dynamic;

    // Create output texture
    texDesc.format = TextureFormat::R32Float;
    mSsaoTex = mRenderDevice.createTexture2D(texDesc);

    // Generate noise texture
    std::uniform_real_distribution<float> randomFloats(0.0F, 1.0F); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoNoise;
    ssaoNoise.resize(16);
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                        randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                        0.0F);
        ssaoNoise[i] = noise;
    }

    texDesc.width = texDesc.height = 4;
    texDesc.format = TextureFormat::RGB16Float;
    texDesc.data[0] = ssaoNoise.data();
    mSsaoNoiseTex = mRenderDevice.createTexture2D(texDesc);

    // Create the framebuffer
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(mSsaoTex);
    mSsaoFb = mRenderDevice.createFramebuffer(fbDesc);
}

void DeferredRenderer::generateSSAONoise()
{
    // Generate kernel samples
    std::uniform_real_distribution<float> randomFloats(0.0F, 1.0F); // random floats between [0.0, 1.0]
    std::default_random_engine generator;

    mSsaoKernel.resize(64);
    for (unsigned int i = 0; i < 64; i++)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                         randomFloats(generator) * 2.0F - 1.0F, // [-1.0, 1.0]
                         randomFloats(generator)                // [ 0.0, 1.0]
        );
        sample = glm::normalize(sample);
        // sample *= randomFloats(generator);
        float scale = static_cast<float>(i) / 64.0F;

        scale = glm::lerp(0.1F, 1.0F, scale * scale);
        sample *= scale;

        mSsaoKernel[i] = sample;
    }
}
