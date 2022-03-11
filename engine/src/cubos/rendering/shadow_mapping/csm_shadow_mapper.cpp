#include <cubos/rendering/shadow_mapping/csm_shadow_mapper.hpp>
#include <fmt/printf.h>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

inline void CSMShadowMapper::setupFramebuffers()
{
    Texture2DArrayDesc spotAtlasDesc;
    spotAtlasDesc.format = TextureFormat::Depth32;
    spotAtlasDesc.width = spotResolution;
    spotAtlasDesc.height = spotResolution;
    spotAtlasDesc.usage = gl::Usage::Dynamic;
    spotAtlasDesc.size = CUBOS_MAX_SPOT_SHADOW_MAPS;
    spotAtlas = renderDevice.createTexture2DArray(spotAtlasDesc);

    Texture2DArrayDesc directionalAtlasDesc;
    directionalAtlasDesc.format = TextureFormat::Depth32;
    directionalAtlasDesc.width = spotResolution;
    directionalAtlasDesc.height = spotResolution;
    directionalAtlasDesc.usage = gl::Usage::Dynamic;
    directionalAtlasDesc.size = CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT * numCascades;
    directionalAtlas = renderDevice.createTexture2DArray(directionalAtlasDesc);

    FramebufferDesc spotFBDesc;
    spotFBDesc.targetCount = 0;
    spotFBDesc.depthStencil.setTexture2DArrayTarget(spotAtlas);
    spotFramebuffer = renderDevice.createFramebuffer(spotFBDesc);

    FramebufferDesc directionalFBDesc;
    directionalFBDesc.targetCount = 0;
    directionalFBDesc.depthStencil.setTexture2DArrayTarget(directionalAtlas);
    directionalFramebuffer = renderDevice.createFramebuffer(directionalFBDesc);
}

inline void CSMShadowMapper::setupPipelines()
{
    ShaderStage spotVertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
        #version 460 core
        layout (location = 0) in uvec3 position;

        uniform ModelMat
        {
            mat4 M;
        };

        uniform Light {
            mat4 L;
        };

        void main()
        {
            gl_Position = L * M * vec4(position, 1.0);
        }
    )");

    ShaderStage spotGeom = renderDevice.createShaderStage(gl::Stage::Geometry, R"(#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int atlasOffset;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gl_Layer = atlasOffset;
        EmitVertex();
    }
    EndPrimitive();
}
    )");

    ShaderStage directionalVertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
        #version 460 core
        layout (location = 0) in uvec3 position;

        uniform ModelMat
        {
            mat4 M;
        };

        void main()
        {
            gl_Position = M * vec4(position, 1.0);
        }
    )");

    ShaderStage directionalGeom =
        renderDevice.createShaderStage(gl::Stage::Geometry, fmt::sprintf(R"(#version 460 core

layout(triangles, invocations = %d) in;
layout(triangle_strip, max_vertices = 3) out;

uniform LightMatrices
{
    mat4 lightMatrices[%d];
};

uniform int atlasOffset;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = lightMatrices[atlasOffset + gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = atlasOffset + gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
    )",
                                                                         numCascades, CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS)
                                                                .c_str());

    ShaderStage pixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
        #version 460 core

        void main()
        {
        }
    )");

    directionalPipeline = renderDevice.createShaderPipeline(directionalVertex, directionalGeom, pixel);

    directionalModelMatrixBP = directionalPipeline->getBindingPoint("ModelMat");
    directionalModelMatrixBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    directionalModelMatrixBP->bind(directionalModelMatrixBuffer);

    directionalLightMatricesBP = directionalPipeline->getBindingPoint("LightMatrices");
    directionalLightMatricesBuffer =
        renderDevice.createConstantBuffer(CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT * numCascades * sizeof(glm::mat4),
                                          nullptr, gl::Usage::Dynamic, gl::BufferStorageType::Small);
    directionalLightMatricesBP->bind(directionalLightMatricesBuffer);

    directionalAtlasOffsetBP = directionalPipeline->getBindingPoint("atlasOffset");

    spotPipeline = renderDevice.createShaderPipeline(spotVertex, spotGeom, pixel);

    spotModelMatrixBP = spotPipeline->getBindingPoint("ModelMat");
    spotModelMatrixBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    spotModelMatrixBP->bind(spotModelMatrixBuffer);

    spotLightBP = spotPipeline->getBindingPoint("Light");
    spotLightBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    spotLightBP->bind(spotLightBuffer);

    spotAtlasOffsetBP = spotPipeline->getBindingPoint("atlasOffset");
}

inline void CSMShadowMapper::createRenderDeviceStates()
{
    RasterStateDesc rasterStateDesc;
    rasterStateDesc.frontFace = gl::Winding::CCW;
    // rasterStateDesc.cullFace = gl::Face::Front;
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

CSMShadowMapper::CSMShadowMapper(RenderDevice& renderDevice, size_t spotResolution, size_t directionalResolution,
                                 size_t numCascades)
    : ShadowMapper(renderDevice), spotResolution(spotResolution), directionalResolution(directionalResolution),
      numCascades(numCascades)
{
    setupFramebuffers();
    setupPipelines();
    createRenderDeviceStates();
}

void CSMShadowMapper::drawModel(const Renderer::DrawRequest& model)
{
    drawRequests.push_back(model);
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& view, const glm::mat4& proj)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

inline void CSMShadowMapper::renderSpotLights()
{
    renderDevice.setShaderPipeline(spotPipeline);
    renderDevice.setFramebuffer(spotFramebuffer);
    renderDevice.setViewport(0, 0, spotResolution, spotResolution);
    renderDevice.clearDepth(1);
    for (int i = 0; i < numSpotLights; ++i)
    {
        auto& light = *(glm::mat4*)spotLightBuffer->map();
        {
            light = spotMatrices[i] =
                glm::perspective(spotLights[i].spotAngle * 2, 1.0f, 0.001f, spotLights[i].range) *
                glm::lookAt(spotLights[i].position,
                            spotLights[i].position + spotLights[i].rotation * glm::vec3(0.0, 0.0, 1.0),
                            glm::vec3(0.0, 1.0, 0.0));

            spotLightBuffer->unmap();
        }
        spotLightBP->bind(spotLightBuffer);

        spotAtlasOffsetBP->setConstant(static_cast<unsigned int>(i));
        for (auto& drawRequest : drawRequests)
        {
            auto& m = *(glm::mat4*)spotModelMatrixBuffer->map();
            {
                m = drawRequest.modelMat;
                spotModelMatrixBuffer->unmap();
            }
            spotModelMatrixBP->bind(spotModelMatrixBuffer);

            Renderer::RendererModel& model = drawRequest.model;

            renderDevice.setVertexArray(model.va);
            renderDevice.setIndexBuffer(model.ib);

            renderDevice.drawTrianglesIndexed(0, model.numIndices);
        }
    }
}

inline void CSMShadowMapper::renderDirectionalLights(const CameraData& camera)
{
    renderDevice.setShaderPipeline(directionalPipeline);

    auto* directionalMatricesPtr = (glm::mat4*)directionalLightMatricesBuffer->map();
    for (int i = 0; i < numDirectionalLights; ++i)
    {
        for (int j = 0; j < numCascades; ++j)
        {
            glm::mat4 persp =
                glm::perspective(camera.FOV, camera.aspectRatio, j == 0 ? camera.nearPlane : cascades[j - 1],
                                 j == numCascades - 1 ? camera.farPlane : cascades[j]);
            std::vector<glm::vec4> worldCorners = getFrustumCornersWorldSpace(camera.viewMatrix, persp);

            glm::vec3 center = glm::vec3(0, 0, 0);
            for (const auto& v : worldCorners)
            {
                center += glm::vec3(v);
            }
            center /= worldCorners.size();

            const auto lightView = glm::lookAt(center - (directionalLights[i].rotation * glm::vec3(0, 0, 1)), center,
                                               glm::vec3(0.0f, 1.0f, 0.0f));

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::min();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::min();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::min();
            for (const auto& v : worldCorners)
            {
                const auto trf = lightView * v;
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            if (minZ < 0)
            {
                minZ *= zMultiplier;
            }
            else
            {
                minZ /= zMultiplier;
            }
            if (maxZ < 0)
            {
                maxZ /= zMultiplier;
            }
            else
            {
                maxZ *= zMultiplier;
            }

            const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

            directionalMatricesPtr[i * numCascades + j] = directionalMatrices[i * numCascades + j] =
                lightProjection * lightView;
        }
    }
    directionalLightMatricesBuffer->unmap();
    directionalLightMatricesBP->bind(directionalLightMatricesBuffer);

    renderDevice.setFramebuffer(directionalFramebuffer);
    renderDevice.setViewport(0, 0, directionalResolution, directionalResolution);
    renderDevice.clearDepth(1);

    for (int i = 0; i < numDirectionalLights; ++i)
    {
        directionalAtlasOffsetBP->setConstant(static_cast<unsigned int>(i * numCascades));
        for (auto it = drawRequests.begin(); it != drawRequests.end(); it++)
        {
            auto& m = *(glm::mat4*)directionalModelMatrixBuffer->map();
            m = it->modelMat;
            directionalModelMatrixBuffer->unmap();
            directionalModelMatrixBP->bind(directionalModelMatrixBuffer);

            Renderer::RendererModel& model = it->model;

            renderDevice.setVertexArray(model.va);
            renderDevice.setIndexBuffer(model.ib);

            renderDevice.drawTrianglesIndexed(0, model.numIndices);
        }
    }
}

void CSMShadowMapper::render(const gl::CameraData& camera)
{
    renderDevice.setRasterState(rasterState);
    renderDevice.setBlendState(blendState);
    renderDevice.setDepthStencilState(depthStencilState);

    renderSpotLights();
    renderDirectionalLights(camera);
}

void CSMShadowMapper::flush()
{
    drawRequests.clear();
    numSpotLights = 0;
    numDirectionalLights = 0;
    numPointLights = 0;
}

void CSMShadowMapper::addLight(const gl::SpotLightData& light)
{
    if (numSpotLights < CUBOS_MAX_SPOT_SHADOW_MAPS)
    {
        spotLights[numSpotLights++] = light;
    }
}

void CSMShadowMapper::addLight(const gl::DirectionalLightData& light)
{
    if (numDirectionalLights < CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT)
    {
        directionalLights[numDirectionalLights++] = light;
    }
}

void CSMShadowMapper::addLight(const gl::PointLightData& light)
{
    if (numPointLights < CUBOS_CSM_MAX_POINT_SHADOW_COUNT)
    {
        pointLights[numPointLights++] = light;
    }
}

ShadowMapper::SpotOutput CSMShadowMapper::getSpotOutput()
{
    SpotOutput output;
    output.mapAtlas = spotAtlas;
    output.matrices = std::vector<glm::mat4>(spotMatrices, spotMatrices + numSpotLights);
    output.numLights = numSpotLights;
    return output;
}

ShadowMapper::DirectionalOutput CSMShadowMapper::getDirectionalOutput()
{
    DirectionalOutput output;
    output.mapAtlas = directionalAtlas;
    output.matrices =
        std::vector<glm::mat4>(directionalMatrices, directionalMatrices + numDirectionalLights * numCascades);
    output.planeDistances = std::vector<float>(cascades, cascades + numCascades - 1);
    output.atlasStride = numCascades;
    output.numLights = numDirectionalLights;
    return output;
}

void CSMShadowMapper::setCascadeDistances(const std::vector<float>& distances)
{
    if (distances.size() > numCascades - 1)
    {
        logWarning("CSMShadowMapper::setCascadeDistances() failed: provided distance vector has a size larger than "
                   "numCascades - 1.");
    }
    std::copy(distances.begin(), distances.end(), cascades);
}
