#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

#include <random>

using namespace cubos;
using namespace cubos::core;
using namespace cubos::core::gl;
using namespace cubos::engine;
using namespace cubos::engine::gl;

/// Deferred renderer grid implementation.
struct DeferredGrid : public engine::gl::impl::RendererGrid
{
    core::gl::VertexArray va;
    core::gl::IndexBuffer ib;
    size_t indexCount;
};

/// Holds the model view matrix sent to the geometry pass pipeline.
struct MVP
{
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 P;
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
static const char* GEOMETRY_PASS_VS = R"glsl(
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
static const char* GEOMETRY_PASS_PS = R"glsl(
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

/// The vertex shader of the lighting pass pipeline.
static const char* LIGHTING_PASS_VS = R"glsl(
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

/// The pixel shader of the lighting pass pipeline.
static const char* LIGHTING_PASS_PS = R"glsl(
#version 330 core

in vec2 fragUv;

uniform sampler2D palette;
uniform sampler2D position;
uniform sampler2D normal;
uniform usampler2D material;

uniform bool ssaoEnabled;
uniform sampler2D ssaoTex;

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

void main()
{
    uint m = texture(material, fragUv).r;
    if (m == 0u) {
        color = vec4(0.0, 0.0, 0.0, 1.0);
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
static const char* SSAO_PASS_VS = R"glsl(
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
static const char* SSAO_PASS_PS = R"glsl(
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
static const char* SSAO_BLUR_PS = R"glsl(
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

deferred::Renderer::Renderer(RenderDevice& renderDevice, glm::uvec2 size) : gl::Renderer(renderDevice, size)
{
    // Create the states.
    RasterStateDesc rasterStateDesc;
    rasterStateDesc.frontFace = Winding::CCW;
    rasterStateDesc.cullFace = Face::Back;
    rasterStateDesc.cullEnabled = true;
    this->geometryRasterState = this->renderDevice.createRasterState(rasterStateDesc);

    BlendStateDesc blendStateDesc;
    this->geometryBlendState = this->renderDevice.createBlendState(blendStateDesc);

    DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = true;
    depthStencilStateDesc.depth.writeEnabled = true;
    this->geometryDepthStencilState = this->renderDevice.createDepthStencilState(depthStencilStateDesc);

    // Create the geometry pipeline.
    auto geometryVS = this->renderDevice.createShaderStage(Stage::Vertex, GEOMETRY_PASS_VS);
    auto geometryPS = this->renderDevice.createShaderStage(Stage::Pixel, GEOMETRY_PASS_PS);
    this->geometryPipeline = this->renderDevice.createShaderPipeline(geometryVS, geometryPS);
    this->mvpBP = this->geometryPipeline->getBindingPoint("MVP");

    // Create the MVP constant buffer.
    mvpBuffer = renderDevice.createConstantBuffer(sizeof(MVP), nullptr, Usage::Dynamic);

    // Create the lighting pipeline.
    auto lightingVS = this->renderDevice.createShaderStage(Stage::Vertex, LIGHTING_PASS_VS);
    auto lightingPS = this->renderDevice.createShaderStage(Stage::Pixel, LIGHTING_PASS_PS);
    this->lightingPipeline = this->renderDevice.createShaderPipeline(lightingVS, lightingPS);
    this->positionBP = this->lightingPipeline->getBindingPoint("position");
    this->normalBP = this->lightingPipeline->getBindingPoint("normal");
    this->materialBP = this->lightingPipeline->getBindingPoint("material");
    this->paletteBP = this->lightingPipeline->getBindingPoint("palette");
    this->lightsBP = this->lightingPipeline->getBindingPoint("Lights");
    this->ssaoEnabledBP = this->lightingPipeline->getBindingPoint("ssaoEnabled");
    this->ssaoTexBP = this->lightingPipeline->getBindingPoint("ssaoTex");

    // Create the SSAO pipeline.
    auto ssaoVS = this->renderDevice.createShaderStage(Stage::Vertex, SSAO_PASS_VS);
    auto ssaoPS = this->renderDevice.createShaderStage(Stage::Pixel, SSAO_PASS_PS);
    this->ssaoPipeline = this->renderDevice.createShaderPipeline(ssaoVS, ssaoPS);
    this->ssaoPositionBP = this->ssaoPipeline->getBindingPoint("position");
    this->ssaoNormalBP = this->ssaoPipeline->getBindingPoint("normal");
    this->ssaoNoiseBP = this->ssaoPipeline->getBindingPoint("noise");
    this->ssaoViewBP = this->ssaoPipeline->getBindingPoint("view");
    this->ssaoProjectionBP = this->ssaoPipeline->getBindingPoint("projection");
    this->ssaoScreenSizeBP = this->ssaoPipeline->getBindingPoint("screenSize");

    // Create the SSAO blur pipeline.
    auto ssaoBlurPS = this->renderDevice.createShaderStage(Stage::Pixel, SSAO_BLUR_PS);
    this->ssaoBlurPipeline = this->renderDevice.createShaderPipeline(ssaoVS, ssaoBlurPS);
    this->ssaoBlurTexBP = this->ssaoBlurPipeline->getBindingPoint("ssaoInput");

    // Create the sampler used to access the palette and the GBuffer textures in the lighting pipeline.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    this->sampler = this->renderDevice.createSampler(samplerDesc);

    // Create the sampler for the SSAO noise texture
    samplerDesc.addressU = samplerDesc.addressV = samplerDesc.addressW = AddressMode::Repeat;
    this->ssaoNoiseSampler = this->renderDevice.createSampler(samplerDesc);

    // Create the palette texture.
    Texture2DDesc texDesc;
    texDesc.width = 256;
    texDesc.height = 256;
    texDesc.format = TextureFormat::RGBA32Float;
    texDesc.usage = Usage::Default;
    this->paletteTex = this->renderDevice.createTexture2D(texDesc);

    // Create the lights constant buffer.
    this->lightsBuffer = this->renderDevice.createConstantBuffer(sizeof(LightsData), nullptr, Usage::Dynamic);

    // Generate a screen quad for the lighting pass.
    generateScreenQuad(this->renderDevice, this->lightingPipeline, this->screenQuadVA);

    // Create the GBuffer.
    this->size = glm::uvec2(0, 0);
    this->onResize(size);
}

RendererGrid deferred::Renderer::upload(const Grid& grid)
{
    auto deferredGrid = std::make_shared<DeferredGrid>();

    // First, triangulate the grid.
    // This may be improved in the future by doing it in a separate thread and only blocking on it when the grid needs
    // to be drawn.
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    triangulate(grid, vertices, indices);

    // Create the vertex array, vertex buffer and index buffer.
    VertexArrayDesc vaDesc;
    vaDesc.elementCount = 3;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = Type::UInt;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = offsetof(Vertex, position);
    vaDesc.elements[0].buffer.stride = sizeof(Vertex);
    vaDesc.elements[1].name = "normal";
    vaDesc.elements[1].type = Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 0;
    vaDesc.elements[1].buffer.offset = offsetof(Vertex, normal);
    vaDesc.elements[1].buffer.stride = sizeof(Vertex);
    vaDesc.elements[2].name = "material";
    vaDesc.elements[2].type = Type::UShort;
    vaDesc.elements[2].size = 1;
    vaDesc.elements[2].buffer.index = 0;
    vaDesc.elements[2].buffer.offset = offsetof(Vertex, material);
    vaDesc.elements[2].buffer.stride = sizeof(Vertex);
    vaDesc.buffers[0] =
        this->renderDevice.createVertexBuffer(vertices.size() * sizeof(Vertex), vertices.data(), Usage::Static);
    vaDesc.shaderPipeline = this->geometryPipeline;
    deferredGrid->va = renderDevice.createVertexArray(vaDesc);
    deferredGrid->ib =
        renderDevice.createIndexBuffer(indices.size() * sizeof(size_t), &indices[0], IndexFormat::UInt, Usage::Static);
    deferredGrid->indexCount = indices.size();

    return deferredGrid;
}

void deferred::Renderer::setPalette(const core::gl::Palette& palette)
{
    // Get the colors from the palette.
    // Magenta is used for non-existent materials in order to easily identify errors.
    std::vector<glm::vec4> data(65536, {1.0f, 0.0f, 1.0f, 1.0f});
    for (size_t i = 0; i < palette.getSize(); ++i)
    {
        if (palette.getData()[i].similarity(Material::Empty) < 1.0f)
        {
            data[i + 1] = palette.getData()[i].color;
        }
    }

    // Update the data on the GPU.
    this->paletteTex->update(0, 0, 256, 256, data.data());
}

bool deferred::Renderer::isSSAOEnabled() const
{
    return this->ssaoEnabled;
}

void deferred::Renderer::setSSAOEnabled(bool ssaoEnabled)
{
    this->ssaoEnabled = ssaoEnabled;
    if (ssaoEnabled)
    {
        createSSAOTextures();
        generateSSAONoise();
    }
}

void deferred::Renderer::onResize(glm::uvec2 size)
{
    // Only resize if the size has changed.
    if (this->size == size)
    {
        return;
    }

    this->size = size;
    Texture2DDesc texDesc;
    texDesc.width = this->size.x;
    texDesc.height = this->size.y;
    texDesc.usage = Usage::Dynamic;

    // Create the position and normal textures.
    texDesc.format = TextureFormat::RGB32Float;
    this->positionTex = this->renderDevice.createTexture2D(texDesc);
    this->normalTex = this->renderDevice.createTexture2D(texDesc);

    // Create the material texture.
    texDesc.format = TextureFormat::R16UInt;
    this->materialTex = this->renderDevice.createTexture2D(texDesc);

    // Create the depth texture.
    texDesc.format = TextureFormat::Depth24Stencil8;
    this->depthTex = renderDevice.createTexture2D(texDesc);

    // Create the framebuffer for the geometry pass.
    FramebufferDesc gBufferDesc;
    gBufferDesc.targetCount = 3;
    gBufferDesc.targets[0].setTexture2DTarget(this->positionTex);
    gBufferDesc.targets[1].setTexture2DTarget(this->normalTex);
    gBufferDesc.targets[2].setTexture2DTarget(this->materialTex);
    gBufferDesc.depthStencil.setTexture2DTarget(this->depthTex);
    this->gBuffer = this->renderDevice.createFramebuffer(gBufferDesc);

    createSSAOTextures();
}

void deferred::Renderer::onRender(const Camera& camera, const Frame& frame, Framebuffer target)
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
    // 5. Lighting pass:
    //   1. Set the lighting pass state.
    //   2. Draw the screen quad.

    // 1. Prepare the MVP matrix.
    MVP mvp;
    mvp.V = camera.view;
    mvp.P = glm::perspective(glm::radians(camera.fovY), float(this->size.x) / float(this->size.y), camera.zNear,
                             camera.zFar);

    // 2. Fill the light buffer with the light data.
    // First map the buffer.
    LightsData& lightData = *static_cast<LightsData*>(this->lightsBuffer->map());
    lightData.numSpotLights = 0;
    lightData.numDirectionalLights = 0;
    lightData.numPointLights = 0;

    // Set the ambient light.
    lightData.ambientLight = glm::vec4(frame.getAmbient(), 1.0f);

    // Spotlights.
    for (auto& light : frame.getSpotLights())
    {
        if (lightData.numSpotLights >= CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT)
        {
            logWarning("deferred::Renderer::onRender() failed: number of spot lights to be drawn "
                       "this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT);
            break;
        }

        lightData.spotLights[lightData.numSpotLights].position = glm::vec4(light.position, 1.0f);
        lightData.spotLights[lightData.numSpotLights].rotation = glm::toMat4(light.rotation);
        lightData.spotLights[lightData.numSpotLights].color = glm::vec4(light.color, 1.0f);
        lightData.spotLights[lightData.numSpotLights].intensity = light.intensity;
        lightData.spotLights[lightData.numSpotLights].range = light.range;
        lightData.spotLights[lightData.numSpotLights].spotCutoff = glm::cos(light.spotAngle);
        lightData.spotLights[lightData.numSpotLights].spotCutoff = glm::cos(light.innerSpotAngle);
        lightData.numSpotLights += 1;
    }

    // Directional lights.
    for (auto& light : frame.getDirectionalLights())
    {
        if (lightData.numDirectionalLights >= CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT)
        {
            logWarning("deferred::Renderer::onRender() failed: number of directional lights to be drawn "
                       "this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT);
            break;
        }

        lightData.directionalLights[lightData.numDirectionalLights].rotation = glm::toMat4(light.rotation);
        lightData.directionalLights[lightData.numDirectionalLights].color = glm::vec4(light.color, 1.0f);
        lightData.directionalLights[lightData.numDirectionalLights].intensity = light.intensity;
        lightData.numDirectionalLights += 1;
    }

    // Point lights.
    for (auto& light : frame.getPointLights())
    {
        if (lightData.numPointLights >= CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT)
        {
            logWarning("deferred::Renderer::onRender() failed: number of point lights to be drawn "
                       "this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT ({}).",
                       CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT);
            break;
        }
        lightData.pointLights[lightData.numPointLights].position = glm::vec4(light.position, 1.0f);
        lightData.pointLights[lightData.numPointLights].color = glm::vec4(light.color, 1.0f);
        lightData.pointLights[lightData.numPointLights].intensity = light.intensity;
        lightData.pointLights[lightData.numPointLights].range = light.range;
        lightData.numPointLights += 1;
    }

    // Unmap the buffer.
    this->lightsBuffer->unmap();

    // 3. Set the renderer state.
    this->renderDevice.setViewport(0, 0, this->size.x, this->size.y);

    // 4. Geometry pass.
    // 4.1. Set the geometry pass state.
    this->renderDevice.setFramebuffer(this->gBuffer);
    this->renderDevice.setRasterState(this->geometryRasterState);
    this->renderDevice.setBlendState(this->geometryBlendState);
    this->renderDevice.setDepthStencilState(this->geometryDepthStencilState);
    this->renderDevice.setShaderPipeline(this->geometryPipeline);
    this->mvpBP->bind(this->mvpBuffer);

    // 4.2. Clear the GBuffer.
    this->renderDevice.clearTargetColor(0, 0.0f, 0.0f, 0.0f, 1.0f);
    this->renderDevice.clearTargetColor(1, 0.0f, 0.0f, 0.0f, 1.0f);
    this->renderDevice.clearTargetColor(2, 0.0f, 0.0f, 0.0f, 0.0f);
    this->renderDevice.clearDepth(1.0f);

    // 4.3. For each draw command:
    for (auto& drawCmd : frame.getDrawCmds())
    {
        // 4.3.1. Update the MVP constant buffer with the model matrix.
        mvp.M = drawCmd.modelMat;
        memcpy(this->mvpBuffer->map(), &mvp, sizeof(MVP));
        this->mvpBuffer->unmap();

        // 4.3.2. Draw the geometry.
        auto grid = std::static_pointer_cast<DeferredGrid>(drawCmd.grid);
        this->renderDevice.setVertexArray(grid->va);
        this->renderDevice.setIndexBuffer(grid->ib);
        this->renderDevice.drawTrianglesIndexed(0, grid->indexCount);
    }

    // 5. SSAO pass.
    if (ssaoEnabled)
    {
        // 5.1. Set the SSAO pass state.
        this->renderDevice.setFramebuffer(ssaoFB);
        this->renderDevice.setRasterState(nullptr);
        this->renderDevice.setBlendState(nullptr);
        this->renderDevice.setDepthStencilState(nullptr);
        this->renderDevice.setShaderPipeline(this->ssaoPipeline);
        this->ssaoPositionBP->bind(this->positionTex);
        this->ssaoPositionBP->bind(this->sampler);
        this->ssaoNormalBP->bind(this->normalTex);
        this->ssaoNormalBP->bind(this->sampler);
        this->ssaoNoiseBP->bind(this->ssaoNoiseTex);
        this->ssaoNoiseBP->bind(this->ssaoNoiseSampler);
        this->ssaoViewBP->setConstant(mvp.V);
        this->ssaoProjectionBP->setConstant(mvp.P);
        this->ssaoScreenSizeBP->setConstant(this->size);

        // Samples
        for (int i = 0; i < 64; i++)
        {
            this->ssaoSamplesBP =
                this->ssaoPipeline->getBindingPoint(std::string("samples[" + std::to_string(i) + "]").c_str());
            this->ssaoSamplesBP->setConstant(ssaoKernel[i]);
        }

        this->renderDevice.setVertexArray(this->screenQuadVA);
        this->renderDevice.drawTriangles(0, 6);

        // 5.2. Set the SSAO blur pass state.
        this->renderDevice.setShaderPipeline(this->ssaoBlurPipeline);
        this->ssaoBlurTexBP->bind(this->ssaoTex);
        this->renderDevice.drawTriangles(0, 6);
    }

    // 6. Lighting pass.
    // 6.1. Set the lighting pass state.
    this->renderDevice.setFramebuffer(target);
    this->renderDevice.setRasterState(nullptr);
    this->renderDevice.setBlendState(nullptr);
    this->renderDevice.setDepthStencilState(nullptr);
    this->renderDevice.setShaderPipeline(this->lightingPipeline);
    this->positionBP->bind(this->positionTex);
    this->positionBP->bind(this->sampler);
    this->normalBP->bind(this->normalTex);
    this->normalBP->bind(this->sampler);
    this->materialBP->bind(this->materialTex);
    this->materialBP->bind(this->sampler);
    this->paletteBP->bind(this->paletteTex);
    this->paletteBP->bind(this->sampler);
    this->lightsBP->bind(this->lightsBuffer);
    this->ssaoEnabledBP->setConstant(this->ssaoEnabled);
    if (this->ssaoEnabledBP)
    {
        this->ssaoTexBP->bind(this->ssaoTex);
        this->ssaoTexBP->bind(this->sampler);
    }

    // 6.2. Draw the screen quad.
    this->renderDevice.setVertexArray(this->screenQuadVA);
    this->renderDevice.drawTriangles(0, 6);

    // Provide custom inputs to the PPS manager.
    this->pps().provideInput(pps::Input::Position, this->positionTex);
    this->pps().provideInput(pps::Input::Normal, this->normalTex);
}

void deferred::Renderer::createSSAOTextures()
{
    Texture2DDesc texDesc;
    texDesc.width = this->size.x;
    texDesc.height = this->size.y;
    texDesc.usage = Usage::Dynamic;

    // Create output texture
    texDesc.format = TextureFormat::R32Float;
    this->ssaoTex = this->renderDevice.createTexture2D(texDesc);

    // Generate noise texture
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoNoise;
    ssaoNoise.resize(16);
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, // [-1.0, 1.0]
                        randomFloats(generator) * 2.0 - 1.0, // [-1.0, 1.0]
                        0.0f);
        ssaoNoise[i] = noise;
    }

    texDesc.width = texDesc.height = 4;
    texDesc.format = TextureFormat::RGB16Float;
    texDesc.data[0] = ssaoNoise.data();
    this->ssaoNoiseTex = this->renderDevice.createTexture2D(texDesc);

    // Create the framebuffer
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(this->ssaoTex);
    this->ssaoFB = this->renderDevice.createFramebuffer(fbDesc);
}

void deferred::Renderer::generateSSAONoise()
{
    // Generate kernel samples
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
    std::default_random_engine generator;

    this->ssaoKernel.resize(64);
    for (unsigned int i = 0; i < 64; i++)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, // [-1.0, 1.0]
                         randomFloats(generator) * 2.0 - 1.0, // [-1.0, 1.0]
                         randomFloats(generator)              // [ 0.0, 1.0]
        );
        sample = glm::normalize(sample);
        // sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        scale = glm::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        this->ssaoKernel[i] = sample;
    }
}
