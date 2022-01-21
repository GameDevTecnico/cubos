#include <cubos/rendering/deferred/deferred_renderer.hpp>

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
                vec4 viewPosition = V * M * vec4(position, 1.0);
                fragPosition = vec3(viewPosition);

                mat3 N = transpose(inverse(mat3(V * M)));
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

    mvpBindingPoint = gBufferPipeline->getBindingPoint("MVP");
    mvpBuffer = renderDevice.createConstantBuffer(3 * sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);

    auto outputVertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
            #version 330 core

            in vec2 position;
            in vec2 uv;

            out vec2 fraguv;

            void main(void)
            {
                gl_Position = vec4(position, 0, 1);
                fraguv = uv;
            }
        )");

    auto outputPixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 330 core

            in vec2 fraguv;

            uniform sampler2D position;
            uniform sampler2D normal;
            uniform usampler2D material;

            out vec4 color;

            void main()
            {
                color = texture(material, fraguv) * texture(normal,fraguv)* texture(position, fraguv) * 0.00001f;
                //color += vec4(vec3(float(texture(material, fraguv).r) / float(0xffff)), 1);
                color += vec4(texture(normal, fraguv).xyz, 1);
            }
        )");

    outputPipeline = renderDevice.createShaderPipeline(outputVertex, outputPixel);

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
    screenVADesc.shaderPipeline = outputPipeline;
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
    // positionTexDesc.data[0] = nullptr;
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

    FramebufferDesc gBufferDesc;
    gBufferDesc.targetCount = 3;
    gBufferDesc.targets[0].setTexture2DTarget(positionTex);
    gBufferDesc.targets[1].setTexture2DTarget(normalTex);
    gBufferDesc.targets[2].setTexture2DTarget(materialTex);
    gBufferDesc.depthStencil = depthTex;

    gBuffer = renderDevice.createFramebuffer(gBufferDesc);

    renderDevice.setShaderPipeline(outputPipeline);

    SamplerDesc positionSamplerDesc;
    positionSamplerDesc.addressU = gl::AddressMode::Clamp;
    positionSamplerDesc.addressV = gl::AddressMode::Clamp;
    positionSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    positionSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    positionSampler = renderDevice.createSampler(positionSamplerDesc);

    outputPositionBP = outputPipeline->getBindingPoint("position");
    outputPositionBP->bind(positionTex);
    outputPositionBP->bind(positionSampler);

    SamplerDesc normalSamplerDesc;
    normalSamplerDesc.addressU = gl::AddressMode::Clamp;
    normalSamplerDesc.addressV = gl::AddressMode::Clamp;
    normalSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    normalSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    normalSampler = renderDevice.createSampler(normalSamplerDesc);

    outputNormalBP = outputPipeline->getBindingPoint("normal");
    outputNormalBP->bind(normalTex);
    outputNormalBP->bind(normalSampler);

    SamplerDesc materialSamplerDesc;
    materialSamplerDesc.addressU = gl::AddressMode::Clamp;
    materialSamplerDesc.addressV = gl::AddressMode::Clamp;
    materialSamplerDesc.magFilter = gl::TextureFilter::Nearest;
    materialSamplerDesc.minFilter = gl::TextureFilter::Nearest;
    materialSampler = renderDevice.createSampler(materialSamplerDesc);

    outputMaterialBP = outputPipeline->getBindingPoint("material");
    outputMaterialBP->bind(materialTex);
    outputMaterialBP->bind(materialSampler);
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

Renderer::ID DeferredRenderer::registerModel(const std::vector<VertexModel>& vertices, std::vector<uint32_t>& indices)
{
    RendererModel model;

    VertexBuffer vb =
        renderDevice.createVertexBuffer(vertices.size() * sizeof(VertexModel), &vertices[0], gl::Usage::Static);

    VertexArrayDesc vaDesc;
    vaDesc.elementCount = 3;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = gl::Type::UInt;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = offsetof(VertexModel, vertex);
    vaDesc.elements[0].buffer.stride = sizeof(VertexModel);
    vaDesc.elements[1].name = "normal";
    vaDesc.elements[1].type = gl::Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 0;
    vaDesc.elements[1].buffer.offset = offsetof(VertexModel, normal);
    vaDesc.elements[1].buffer.stride = sizeof(VertexModel);
    vaDesc.elements[2].name = "material";
    vaDesc.elements[2].type = gl::Type::UShort;
    vaDesc.elements[2].size = 1;
    vaDesc.elements[2].buffer.index = 0;
    vaDesc.elements[2].buffer.offset = offsetof(VertexModel, material);
    vaDesc.elements[2].buffer.stride = sizeof(VertexModel);
    vaDesc.buffers[0] = vb;
    vaDesc.shaderPipeline = gBufferPipeline;

    model.va = renderDevice.createVertexArray(vaDesc);
    model.ib = renderDevice.createIndexBuffer(indices.size() * sizeof(size_t), &indices[0], gl::IndexFormat::UInt,
                                              gl::Usage::Static);
    model.numIndices = indices.size();

    models.push_back(model);
    return models.size() - 1;
}

void DeferredRenderer::drawModel(Renderer::ID modelID, glm::mat4 modelMat)
{
    drawRequests.emplace_back(models[modelID], modelMat);
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
    renderDevice.clearTargetColor(0, 0, 0, FLT_MAX, 1);
    renderDevice.clearTargetColor(1, 0, 0, 0, 1);
    renderDevice.clearTargetColor(2, 0, 0, 0, 0);
    renderDevice.clearDepth(1);

    mvpBindingPoint->bind(mvpBuffer);

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

    renderDevice.setShaderPipeline(outputPipeline);
    renderDevice.setFramebuffer(camera.target);
    renderDevice.setViewport(0, 0, sz.x, sz.y);
    renderDevice.clearColor(0, 0, 0, 0);
    renderDevice.clearDepth(1);

    renderDevice.setRasterState(nullptr);
    renderDevice.setBlendState(nullptr);
    renderDevice.setDepthStencilState(nullptr);

    renderDevice.setVertexArray(screenVertexArray);
    renderDevice.setIndexBuffer(screenIndexBuffer);
    renderDevice.drawTrianglesIndexed(0, 6);
}
