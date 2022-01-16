#include <cubos/rendering/deferred/deferred_renderer.hpp>

using namespace cubos::gl;
using namespace cubos::rendering;

inline void DeferredRenderer::createPipeline()
{
    auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
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
                fragNormal = N * (normal);

                gl_Position = P * viewPosition;
            }
        )");

    auto ps = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
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

    shaderPipeline = renderDevice.createShaderPipeline(vs, ps);
}

inline void DeferredRenderer::setupFrameBuffers()
{
    auto sz = window.getFramebufferSize();

    Texture2DDesc positionTexDesc;
    positionTexDesc.format = TextureFormat::RGBA32Float;
    positionTexDesc.width = sz.x;
    positionTexDesc.height = sz.y;
    // positionTexDesc.data[0] = nullptr;
    positionTex = renderDevice.createTexture2D(positionTexDesc);

    Texture2DDesc normalTexDesc;
    normalTexDesc.format = TextureFormat::RGBA32Float;
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
}

DeferredRenderer::DeferredRenderer(io::Window& window) : Renderer(window)
{
    createPipeline();
    setupFrameBuffers();
}

Renderer::ID DeferredRenderer::registerModel(const std::vector<glm::uvec3>& vertices,
                                             const std::vector<glm::vec3>& normals,
                                             const std::vector<uint32_t>& materials, std::vector<uint32_t>& indices)
{
    RendererModel model;

    VertexBuffer vb =
        renderDevice.createVertexBuffer(vertices.size() * sizeof(glm::uvec3), &vertices[0], gl::Usage::Static);
    VertexBuffer nb =
        renderDevice.createVertexBuffer(normals.size() * sizeof(glm::vec3), &normals[0], gl::Usage::Static);
    VertexBuffer mb =
        renderDevice.createVertexBuffer(normals.size() * sizeof(uint32_t), &materials[0], gl::Usage::Static);

    VertexArrayDesc vaDesc;
    vaDesc.elementCount = 2;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = gl::Type::UInt;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = 0;
    vaDesc.elements[0].buffer.stride = sizeof(glm::uvec3);
    vaDesc.elements[1].name = "normal";
    vaDesc.elements[1].type = gl::Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 1;
    vaDesc.elements[1].buffer.offset = 0;
    vaDesc.elements[1].buffer.stride = sizeof(glm::vec3);
    vaDesc.elements[1].name = "material";
    vaDesc.elements[1].type = gl::Type::UInt;
    vaDesc.elements[1].size = 1;
    vaDesc.elements[1].buffer.index = 2;
    vaDesc.elements[1].buffer.offset = 0;
    vaDesc.elements[1].buffer.stride = sizeof(uint32_t);
    vaDesc.buffers[0] = vb;
    vaDesc.buffers[1] = nb;
    vaDesc.buffers[2] = mb;
    vaDesc.shaderPipeline = shaderPipeline;

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
void DeferredRenderer::render()
{
}
