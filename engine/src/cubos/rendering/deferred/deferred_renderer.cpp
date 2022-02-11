#include <cubos/gl/vertex.hpp>
#include <cubos/rendering/deferred/deferred_renderer.hpp>
#include <cubos/log.hpp>

#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

inline void DeferredRenderer::createShaderPipelines()
{
    auto gBufferVertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
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
        )");

    auto gBufferPixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
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
        )");

    gBufferPipeline = renderDevice.createShaderPipeline(gBufferVertex, gBufferPixel);

    mvpBP = gBufferPipeline->getBindingPoint("MVP");
    mvpBuffer = renderDevice.createConstantBuffer(3 * sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);

    auto outputVertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
            #version 330 core

            in vec4 position;
            in vec2 uv;

            out vec2 fraguv;

            void main(void)
            {
                gl_Position = position;
                fraguv = uv;
            }
        )");

    auto outputSmallPixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 430 core

            in vec2 fraguv;

            uniform sampler2D position;
            uniform sampler2D normal;
            uniform usampler2D material;

            struct Material
            {
                vec4 color;
            };

            uniform palette
            {
                 Material materials[gl_MaxFragmentUniformComponents / 4];
            };

            struct SpotLightData
            {
                vec4 position;
                mat4 rotation;
                vec4 color;
                float intensity;
                float range;
                float spotCutoff;
                float innerSpotCutoff;
            };

            struct DirectionalLightData
            {
                mat4 rotation;
                vec4 color;
                float intensity;
            };

            struct PointLightData
            {
                vec4 position;
                vec4 color;
                float intensity;
                float range;
            };

            layout(std140) uniform Lights
            {
                SpotLightData spotLights[128];
                DirectionalLightData directionalLights[128];
                PointLightData pointLights[128];
                uint numSpotLights;
                uint numDirectionalLights;
                uint numPointLights;
            };

            layout(location = 0) out vec4 color;

            float remap(float value, float min1, float max1, float min2, float max2) {
                return max2 + (value - min1) * (max2 - min2) / (max1 - min1);
            }

            vec3 spotLightCalc(vec3 fragPos, vec3 fragNormal, SpotLightData light) {
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

            vec3 directionalLightCalc(vec3 fragNormal, DirectionalLightData light)
            {
                return max(dot(fragNormal, -vec3(light.rotation * vec4(0,0,1,1))), 0) * light.intensity * vec3(light.color);
            }

            vec3 pointLightCalc(vec3 fragPos, vec3 fragNormal, PointLightData light) {
                vec3 toLight = vec3(light.position) - fragPos;
                float r = length(toLight) / light.range;
                if (r < 1) {
                    float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
                    float diffuse = max(dot(fragNormal, vec3(normalize(toLight))), 0);
                    return attenuation * diffuse * light.intensity * vec3(light.color);
                }
                return vec3(0);
            }

            void main()
            {
                uint m = texture(material, fraguv).r;
                if (m == 0u) {
                    discard;
                }
                vec3 albedo = materials[m - 1u].color.rgb;
                vec3 lighting = vec3(0);
                vec3 fragPos = texture(position, fraguv).xyz;
                vec3 fragNormal = texture(normal, fraguv).xyz;
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
        )");

    auto outputLargePixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 430 core

            in vec2 fraguv;

            uniform sampler2D position;
            uniform sampler2D normal;
            uniform usampler2D material;

            struct Material
            {
                vec4 color;
            };

            layout(std430) buffer palette {
                Material materials[];
            };

            struct SpotLightData
            {
                vec4 position;
                mat4 rotation;
                vec4 color;
                float intensity;
                float range;
                float spotCutoff;
                float innerSpotCutoff;
            };

            struct DirectionalLightData
            {
                mat4 rotation;
                vec4 color;
                float intensity;
            };

            struct PointLightData
            {
                vec4 position;
                vec4 color;
                float intensity;
                float range;
            };

            layout(std140) uniform Lights
            {
                SpotLightData spotLights[128];
                DirectionalLightData directionalLights[128];
                PointLightData pointLights[128];
                uint numSpotLights;
                uint numDirectionalLights;
                uint numPointLights;
            };

            layout(location = 0) out vec4 color;

            float remap(float value, float min1, float max1, float min2, float max2) {
                return max2 + (value - min1) * (max2 - min2) / (max1 - min1);
            }

            vec3 spotLightCalc(vec3 fragPos, vec3 fragNormal, SpotLightData light) {
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

            vec3 directionalLightCalc(vec3 fragNormal, DirectionalLightData light)
            {
                return max(dot(fragNormal, -vec3(light.rotation * vec4(0,0,1,1))), 0) * light.intensity * vec3(light.color);
            }

            vec3 pointLightCalc(vec3 fragPos, vec3 fragNormal, PointLightData light) {
                vec3 toLight = vec3(light.position) - fragPos;
                float r = length(toLight) / light.range;
                if (r < 1) {
                    float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
                    float diffuse = max(dot(fragNormal, vec3(normalize(toLight))), 0);
                    return attenuation * diffuse * light.intensity * vec3(light.color);
                }
                return vec3(0);
            }

            void main()
            {
                uint m = texture(material, fraguv).r;
                if (m == 0u) {
                    discard;
                }
                vec3 albedo = materials[m - 1u].color.rgb;
                vec3 lighting = vec3(0);
                vec3 fragPos = texture(position, fraguv).xyz;
                vec3 fragNormal = texture(normal, fraguv).xyz;
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
        )");

    smallOutputPipeline = renderDevice.createShaderPipeline(outputVertex, outputSmallPixel);
    largeOutputPipeline = renderDevice.createShaderPipeline(outputVertex, outputLargePixel);

    outputLightBlockBuffer = renderDevice.createConstantBuffer(sizeof(LightBlock), nullptr, gl::Usage::Dynamic);

    renderDevice.setShaderPipeline(smallOutputPipeline);
    smallOutputLightBlockBP = smallOutputPipeline->getBindingPoint("Lights");

    renderDevice.setShaderPipeline(largeOutputPipeline);
    largeOutputLightBlockBP = largeOutputPipeline->getBindingPoint("Lights");

    float screenVerts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, +1.0f, 0.0f, 1.0f, +1.0f, +1.0f, 1.0f, 1.0f, +1.0f, -1.0f, 1.0f, 0.0f,
    };
    auto screenVertexBuffer = renderDevice.createVertexBuffer(sizeof(screenVerts), screenVerts, gl::Usage::Static);

    VertexArrayDesc screenVADesc;
    screenVADesc.elementCount = 2;
    screenVADesc.elements[0].name = "position";
    screenVADesc.elements[0].type = gl::Type::Float;
    screenVADesc.elements[0].size = 2;
    screenVADesc.elements[0].buffer.index = 0;
    screenVADesc.elements[0].buffer.offset = 0;
    screenVADesc.elements[0].buffer.stride = 4 * sizeof(float);
    screenVADesc.elements[1].name = "uv";
    screenVADesc.elements[1].type = gl::Type::Float;
    screenVADesc.elements[1].size = 2;
    screenVADesc.elements[1].buffer.index = 0;
    screenVADesc.elements[1].buffer.offset = 2 * sizeof(float);
    screenVADesc.elements[1].buffer.stride = 4 * sizeof(float);
    screenVADesc.buffers[0] = screenVertexBuffer;
    screenVADesc.shaderPipeline = smallOutputPipeline;
    screenVertexArray = renderDevice.createVertexArray(screenVADesc);

    unsigned int screenIndices[] = {
        0, 1, 2, 2, 3, 0,
    };
    screenIndexBuffer =
        renderDevice.createIndexBuffer(sizeof(screenIndices), screenIndices, gl::IndexFormat::UInt, gl::Usage::Static);
}

inline void DeferredRenderer::setupFrameBuffers()
{
    auto sz = window.getFramebufferSize();

    Texture2DDesc positionTexDesc;
    positionTexDesc.format = TextureFormat::RGB32Float;
    positionTexDesc.width = sz.x;
    positionTexDesc.height = sz.y;
    positionTex = renderDevice.createTexture2D(positionTexDesc);

    Texture2DDesc normalTexDesc;
    normalTexDesc.format = TextureFormat::RGB32Float;
    normalTexDesc.width = sz.x;
    normalTexDesc.height = sz.y;
    normalTex = renderDevice.createTexture2D(normalTexDesc);

    Texture2DDesc materialTexDesc;
    materialTexDesc.format = TextureFormat::R16UInt;
    materialTexDesc.width = sz.x;
    materialTexDesc.height = sz.y;
    materialTex = renderDevice.createTexture2D(materialTexDesc);

    Texture2DDesc depthTexDesc;
    depthTexDesc.format = TextureFormat::Depth24Stencil8;
    depthTexDesc.width = sz.x;
    depthTexDesc.height = sz.y;
    depthTex = renderDevice.createTexture2D(depthTexDesc);

    Texture2DDesc outputTexDesc;
    outputTexDesc.format = TextureFormat::RGBA32Float;
    outputTexDesc.width = sz.x;
    outputTexDesc.height = sz.y;
    outputTexture1 = renderDevice.createTexture2D(outputTexDesc);
    outputTexture2 = renderDevice.createTexture2D(outputTexDesc);

    FramebufferDesc gBufferDesc;
    gBufferDesc.targetCount = 3;
    gBufferDesc.targets[0].setTexture2DTarget(positionTex);
    gBufferDesc.targets[1].setTexture2DTarget(normalTex);
    gBufferDesc.targets[2].setTexture2DTarget(materialTex);
    gBufferDesc.depthStencil = depthTex;

    gBuffer = renderDevice.createFramebuffer(gBufferDesc);

    FramebufferDesc outputFramebufferDesc;
    outputFramebufferDesc.targetCount = 1;
    outputFramebufferDesc.targets[0].setTexture2DTarget(outputTexture1);
    outputFramebuffer1 = renderDevice.createFramebuffer(outputFramebufferDesc);
    outputFramebufferDesc.targets[0].setTexture2DTarget(outputTexture2);
    outputFramebuffer2 = renderDevice.createFramebuffer(outputFramebufferDesc);

    SamplerDesc positionSamplerDesc;
    positionSamplerDesc.addressU = gl::AddressMode::Clamp;
    positionSamplerDesc.addressV = gl::AddressMode::Clamp;
    positionSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    positionSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    positionSampler = renderDevice.createSampler(positionSamplerDesc);

    SamplerDesc normalSamplerDesc;
    normalSamplerDesc.addressU = gl::AddressMode::Clamp;
    normalSamplerDesc.addressV = gl::AddressMode::Clamp;
    normalSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    normalSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    normalSampler = renderDevice.createSampler(normalSamplerDesc);

    SamplerDesc materialSamplerDesc;
    materialSamplerDesc.addressU = gl::AddressMode::Clamp;
    materialSamplerDesc.addressV = gl::AddressMode::Clamp;
    materialSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    materialSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    materialSampler = renderDevice.createSampler(materialSamplerDesc);

    renderDevice.setShaderPipeline(smallOutputPipeline);

    smallOutputPositionBP = smallOutputPipeline->getBindingPoint("position");
    smallOutputPositionBP->bind(positionTex);
    smallOutputPositionBP->bind(positionSampler);

    smallOutputNormalBP = smallOutputPipeline->getBindingPoint("normal");
    smallOutputNormalBP->bind(normalTex);
    smallOutputNormalBP->bind(normalSampler);

    smallOutputMaterialBP = smallOutputPipeline->getBindingPoint("material");
    smallOutputMaterialBP->bind(materialTex);
    smallOutputMaterialBP->bind(materialSampler);

    smallOutputPaletteBP = smallOutputPipeline->getBindingPoint("palette");

    renderDevice.setShaderPipeline(largeOutputPipeline);

    largeOutputPositionBP = largeOutputPipeline->getBindingPoint("position");
    largeOutputPositionBP->bind(positionTex);
    largeOutputPositionBP->bind(positionSampler);

    largeOutputNormalBP = largeOutputPipeline->getBindingPoint("normal");
    largeOutputNormalBP->bind(normalTex);
    largeOutputNormalBP->bind(normalSampler);

    largeOutputMaterialBP = largeOutputPipeline->getBindingPoint("material");
    largeOutputMaterialBP->bind(materialTex);
    largeOutputMaterialBP->bind(materialSampler);

    largeOutputPaletteBP = largeOutputPipeline->getBindingPoint("palette");
}

inline void DeferredRenderer::createRenderDeviceStates()
{
    RasterStateDesc rasterStateDesc;
    rasterStateDesc.frontFace = gl::Winding::CCW;
    rasterStateDesc.cullFace = gl::Face::Back;
    rasterStateDesc.cullEnabled = true;
    rasterState = renderDevice.createRasterState(rasterStateDesc);

    BlendStateDesc blendStateDesc;
    // blendStateDesc.blendEnabled = true;
    blendState = renderDevice.createBlendState(blendStateDesc);

    DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = true;
    depthStencilStateDesc.depth.writeEnabled = true;
    depthStencilState = renderDevice.createDepthStencilState(depthStencilStateDesc);
}

DeferredRenderer::DeferredRenderer(io::Window& window) : Renderer(window)
{
    createShaderPipelines();
    setupFrameBuffers();
    createRenderDeviceStates();
}

Renderer::ModelID DeferredRenderer::registerModel(const std::vector<cubos::gl::Vertex>& vertices,
                                                  std::vector<uint32_t>& indices)
{
    return registerModelInternal(vertices, indices, gBufferPipeline);
}

void DeferredRenderer::drawLight(const SpotLightData& light)
{
    if (lights.numSpotLights >= CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT)
    {
        logWarning("DeferredRenderer::drawLight(const SpotLightData& light) failed: number of spot lights to be drawn "
                   "this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT.");
        return;
    }
    lights.spotLights[lights.numSpotLights++] = light;
}

void DeferredRenderer::drawLight(const DirectionalLightData& light)
{
    if (lights.numDirectionalLights >= CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT)
    {
        logWarning("DeferredRenderer::drawLight(const DirectionalLightData& light) failed: number of directional "
                   "lights to be drawn this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT.");
        return;
    }
    lights.directionalLights[lights.numDirectionalLights++] = light;
}

void DeferredRenderer::drawLight(const PointLightData& light)
{
    if (lights.numDirectionalLights >= CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT)
    {
        logWarning("DeferredRenderer::drawLight(const PointLightData& light) failed: number of point lights to be "
                   "drawn this frame exceeds CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT.");
        return;
    }
    lights.pointLights[lights.numPointLights++] = light;
}

void DeferredRenderer::render(const CameraData& camera, bool usePostProcessing)
{

    renderDevice.setFramebuffer(camera.target);
    auto sz = window.getFramebufferSize();

    renderDevice.setShaderPipeline(gBufferPipeline);
    renderDevice.setFramebuffer(gBuffer);
    renderDevice.setViewport(0, 0, sz.x, sz.y);

    renderDevice.setRasterState(rasterState);
    renderDevice.setBlendState(blendState);
    renderDevice.setDepthStencilState(depthStencilState);
    renderDevice.clearTargetColor(0, 0, 0, 0, 1);
    renderDevice.clearTargetColor(1, 0, 0, 0, 1);
    renderDevice.clearTargetColor(2, 0, 0, 0, 0);
    renderDevice.clearDepth(1);

    mvpBP->bind(mvpBuffer);

    auto& mvp = *(MVP*)mvpBuffer->map();
    mvp.V = camera.viewMatrix;
    mvp.P = camera.perspectiveMatrix;
    mvpBuffer->unmap();

    for (auto it = drawRequests.begin(); it != drawRequests.end(); it++)
    {
        auto& m = *(glm::mat4*)mvpBuffer->map();
        m = it->modelMat;
        mvpBuffer->unmap();

        RendererModel& model = it->model;

        renderDevice.setVertexArray(model.va);
        renderDevice.setIndexBuffer(model.ib);

        renderDevice.drawTrianglesIndexed(0, model.numIndices);
    }

    bool doPostProcessing = usePostProcessing && !postProcessingPasses.empty();

    if (doPostProcessing)
    {
        renderDevice.setFramebuffer(outputFramebuffer1);
    }
    else
    {
        renderDevice.setFramebuffer(camera.target);
    }

    ShaderPipeline pp;
    switch (currentPalette->getStorageTypeHint())
    {
    case BufferStorageType::Small:
        pp = smallOutputPipeline;
        renderDevice.setShaderPipeline(pp);
        smallOutputPaletteBP->bind(currentPalette);
        smallOutputLightBlockBP->bind(outputLightBlockBuffer);
        {
            auto& lightBlock = *(LightBlock*)outputLightBlockBuffer->map();
            lightBlock = lights;
            outputLightBlockBuffer->unmap();
        }
        smallOutputPositionBP->bind(positionTex);
        smallOutputNormalBP->bind(normalTex);
        smallOutputMaterialBP->bind(materialTex);
        break;
    case BufferStorageType::Large:
        pp = largeOutputPipeline;
        renderDevice.setShaderPipeline(pp);
        largeOutputPaletteBP->bind(currentPalette);
        largeOutputLightBlockBP->bind(outputLightBlockBuffer);
        {
            auto& lightBlock = *(LightBlock*)outputLightBlockBuffer->map();
            lightBlock = lights;
            outputLightBlockBuffer->unmap();
        }
        largeOutputPositionBP->bind(positionTex);
        largeOutputNormalBP->bind(normalTex);
        largeOutputMaterialBP->bind(materialTex);
        break;
    }

    renderDevice.setViewport(0, 0, sz.x, sz.y);
    renderDevice.setRasterState(nullptr);
    renderDevice.setBlendState(nullptr);
    renderDevice.setDepthStencilState(nullptr);
    renderDevice.clearTargetColor(0, 0, 0, 0, 1);

    renderDevice.setVertexArray(screenVertexArray);
    renderDevice.setIndexBuffer(screenIndexBuffer);
    renderDevice.drawTrianglesIndexed(0, 6);

    if (doPostProcessing)
    {
        auto last = std::prev(postProcessingPasses.end());
        for (auto it = postProcessingPasses.begin(); it != last; it++)
        {
            it->get().execute(*this, outputTexture1, outputFramebuffer2);

            // swap input and output
            auto tempTex = outputTexture1;
            outputTexture1 = outputTexture2;
            outputTexture2 = tempTex;
            auto tempFB = outputFramebuffer1;
            outputFramebuffer1 = outputFramebuffer2;
            outputFramebuffer2 = tempFB;
        }
        last->get().execute(*this, outputTexture1, camera.target);
    }
}

void DeferredRenderer::flush()
{
    Renderer::flush();
    lights.numSpotLights = 0;
    lights.numDirectionalLights = 0;
    lights.numPointLights = 0;
}

DeferredRenderer::LightBlock::SpotLightData::SpotLightData(const gl::SpotLightData& light)
    : position(glm::vec4(light.position, 1)), rotation(glm::toMat4(light.rotation)), color(glm::vec4(light.color, 1)),
      intensity(light.intensity), range(light.range), spotCutoff(cos(light.spotAngle)),
      innerSpotCutoff(cos(light.innerSpotAngle))
{
}

DeferredRenderer::LightBlock::DirectionalLightData::DirectionalLightData(const gl::DirectionalLightData& light)
    : rotation(glm::toMat4(light.rotation)), color(glm::vec4(light.color, 1)), intensity(light.intensity)
{
}

DeferredRenderer::LightBlock::PointLightData::PointLightData(const gl::PointLightData& light)
    : position(glm::vec4(light.position, 1)), color(glm::vec4(light.color, 1)), intensity(light.intensity),
      range(light.range)
{
}
