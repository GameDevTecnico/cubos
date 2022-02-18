#include <cubos/gl/util.hpp>

using namespace cubos;
using namespace cubos::gl;

void gl::generateScreenQuad(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexArray& va,
                            IndexBuffer& ib)
{
    float screenVerts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, +1.0f, 0.0f, 1.0f, +1.0f, +1.0f, 1.0f, 1.0f, +1.0f, -1.0f, 1.0f, 0.0f,
    };
    VertexBuffer vb = renderDevice.createVertexBuffer(sizeof(screenVerts), screenVerts, gl::Usage::Static);

    unsigned int screenIndices[] = {
        0, 1, 2, 2, 3, 0,
    };
    ib = renderDevice.createIndexBuffer(sizeof(screenIndices), screenIndices, gl::IndexFormat::UInt, gl::Usage::Static);

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
    screenVADesc.buffers[0] = vb;
    screenVADesc.shaderPipeline = pipeline;
    va = renderDevice.createVertexArray(screenVADesc);
}
