#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/util.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace cubos;
using namespace cubos::core;
using namespace cubos::core::gl;
using namespace cubos::engine;
using namespace cubos::engine::gl;

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
    return texelFetch(palette, ivec2(material % 256, material / 256));
}

void main()
{
    uint m = texture(material, fragUv).r;
    if (m == 0u) {
        discard;
    }
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
    color = vec4(albedo * lighting, 1);
}
)glsl";

deferred::Renderer::Renderer(RenderDevice& renderDevice, glm::uvec2 size) : gl::Renderer(renderDevice, size)
{
    // Create the render device states.
    RasterStateDesc rasterStateDesc;
    rasterStateDesc.frontFace = Winding::CCW;
    rasterStateDesc.cullFace = Face::Back;
    rasterStateDesc.cullEnabled = true;
    this->rasterState = this->renderDevice.createRasterState(rasterStateDesc);

    BlendStateDesc blendStateDesc;
    this->blendState = this->renderDevice.createBlendState(blendStateDesc);

    DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = true;
    depthStencilStateDesc.depth.writeEnabled = true;
    this->depthStencilState = this->renderDevice.createDepthStencilState(depthStencilStateDesc);

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

    // Create the sampler used to access the palette and the GBuffer textures in the lighting pipeline.
    SamplerDesc samplerDesc;
    samplerDesc.addressU = AddressMode::Clamp;
    samplerDesc.addressV = AddressMode::Clamp;
    samplerDesc.magFilter = TextureFilter::Nearest;
    samplerDesc.minFilter = TextureFilter::Nearest;
    this->sampler = this->renderDevice.createSampler(samplerDesc);

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

Renderer::GridID deferred::Renderer::upload(const Grid& grid)
{
    GpuGrid gpuGrid;

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
    gpuGrid.va = renderDevice.createVertexArray(vaDesc);
    gpuGrid.ib =
        renderDevice.createIndexBuffer(indices.size() * sizeof(size_t), &indices[0], IndexFormat::UInt, Usage::Static);
    gpuGrid.indexCount = indices.size();

    // Add it to the vector of grids.
    this->grids.push_back(gpuGrid);
    return this->grids.size() - 1;
}

void deferred::Renderer::free(GridID grid)
{
    this->grids[grid].va = nullptr;
    this->grids[grid].ib = nullptr;
    this->grids[grid].indexCount = 0;
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

    // Update the data to the GPU.
    this->paletteTex->update(1, 0, 256, 256, &data[0]);
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

        lightData.directionalLights[lightData.numDirectionalLights].rotation = glm::toMat4(light.rotation, 0.0f);
        lightData.directionalLights[lightData.numDirectionalLights].color = glm::vec4(light.color, 1.0f);
        lightData.directionalLights[lightData.numDirectionalLights].intensity = light.intensity;
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
    }

    // Unmap the buffer.
    this->lightsBuffer->unmap();

    // 3. Set the renderer state.
    this->renderDevice.setRasterState(rasterState);
    this->renderDevice.setBlendState(blendState);
    this->renderDevice.setDepthStencilState(depthStencilState);
    this->renderDevice.setViewport(0, 0, this->size.x, this->size.y);

    // 4. Geometry pass.
    // 4.1. Set the geometry pass state.
    this->renderDevice.setFramebuffer(this->gBuffer);
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
        GpuGrid& grid = this->grids[drawCmd.grid];
        this->renderDevice.setVertexArray(grid.va);
        this->renderDevice.setIndexBuffer(grid.ib);
        this->renderDevice.drawTrianglesIndexed(0, grid.indexCount);
    }

    // 5. Lighting pass.
    // 5.1. Set the lighting pass state.
    this->renderDevice.setFramebuffer(target);
    this->renderDevice.setShaderPipeline(this->lightingPipeline);
    this->positionBP->bind(this->positionTex);
    this->normalBP->bind(this->normalTex);
    this->materialBP->bind(this->materialTex);
    this->paletteBP->bind(this->paletteTex);
    this->lightsBP->bind(this->lightsBuffer);

    // 5.2. Draw the screen quad.
    this->renderDevice.setVertexArray(this->screenQuadVA);
    this->renderDevice.drawTriangles(0, 6);
}
