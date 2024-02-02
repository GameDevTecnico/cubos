#include <cubos/core/gl/util.hpp>

using namespace cubos::core;
using namespace cubos::core::gl;

void gl::generateScreenQuad(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexArray& va)
{
    // The vertices of the quad (positions + uvs)
    float screenVerts[] = {
        -1.0F, -1.0F, 0.0F, 0.0F, -1.0F, +1.0F, 0.0F, 1.0F, +1.0F, +1.0F, 1.0F, 1.0F,
        +1.0F, +1.0F, 1.0F, 1.0F, +1.0F, -1.0F, 1.0F, 0.0F, -1.0F, -1.0F, 0.0F, 0.0F,
    };

    // Create the array and vertex buffer.
    VertexArrayDesc desc;
    desc.elementCount = 2;
    desc.elements[0].name = "position";
    desc.elements[0].type = gl::Type::Float;
    desc.elements[0].size = 2;
    desc.elements[0].buffer.index = 0;
    desc.elements[0].buffer.offset = 0;
    desc.elements[0].buffer.stride = 4 * sizeof(float);
    desc.elements[1].name = "uv";
    desc.elements[1].type = gl::Type::Float;
    desc.elements[1].size = 2;
    desc.elements[1].buffer.index = 0;
    desc.elements[1].buffer.offset = 2 * sizeof(float);
    desc.elements[1].buffer.stride = 4 * sizeof(float);
    desc.buffers[0] = renderDevice.createVertexBuffer(sizeof(screenVerts), screenVerts, gl::Usage::Static);
    desc.shaderPipeline = pipeline;
    va = renderDevice.createVertexArray(desc);
}
