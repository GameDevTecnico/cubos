#include "renderer.hpp"
#include <string>

#include <cubos/core/ecs/reflection.hpp>

using cubos::engine::GizmosRenderer;

using namespace cubos::core::gl;

CUBOS_REFLECT_IMPL(GizmosRenderer)
{
    return core::ecs::TypeBuilder<GizmosRenderer>("cubos::engine::GizmosRenderer").build();
}

void GizmosRenderer::initDrawPipeline()
{
    auto vs = renderDevice->createShaderStage(Stage::Vertex, R"(
            in vec3 position;

            uniform MVP
            {
                mat4 mvp;
            };

            void main()
            {
                gl_Position = mvp * vec4(position, 1.0f);
            }
        )");

    auto ps = renderDevice->createShaderStage(Stage::Pixel, R"(
            out vec4 color;
            
            uniform vec3 objColor;

            void main()
            {
                color = vec4(objColor, 1.0f);
            }
        )");

    drawPipeline = renderDevice->createShaderPipeline(vs, ps);
}

void GizmosRenderer::initIdPipeline()
{
    auto vs = renderDevice->createShaderStage(Stage::Vertex, R"(
            in vec3 position;

            uniform MVP
            {
                mat4 mvp;
            };

            void main()
            {
                gl_Position = mvp * vec4(position, 1.0f);
            }
        )");

    auto ps = renderDevice->createShaderStage(Stage::Pixel, R"(
            uniform uint gizmo;
            
            out uvec2 idOutput;

            void main()
            {
                idOutput.r = (gizmo >> 16U);
                idOutput.g = (gizmo & 65535U);
            }
        )");

    idPipeline = renderDevice->createShaderPipeline(vs, ps);
}

void GizmosRenderer::init(RenderDevice* currentRenderDevice)
{
    renderDevice = currentRenderDevice;

    cubos::core::gl::DepthStencilStateDesc dss;
    dss.depth.enabled = true;
    dss.depth.writeEnabled = true;
    dss.depth.compare = Compare::LEqual;
    doDepthCheckStencilState = renderDevice->createDepthStencilState(dss);
    dss.depth.enabled = false;
    dss.depth.writeEnabled = false;
    noDepthCheckStencilState = renderDevice->createDepthStencilState(dss);

    initDrawPipeline();
    initIdPipeline();

    initLinePrimitive();
    initBoxPrimitive();
    initCutConePrimitive();
    initRingPrimitive();
}

void GizmosRenderer::initLinePrimitive()
{
    linePrimitive.vaDesc.elementCount = 1;
    linePrimitive.vaDesc.elements[0].name = "position";
    linePrimitive.vaDesc.elements[0].type = Type::Float;
    linePrimitive.vaDesc.elements[0].size = 3;
    linePrimitive.vaDesc.elements[0].buffer.index = 0;
    linePrimitive.vaDesc.elements[0].buffer.offset = 0;
    linePrimitive.vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
    linePrimitive.vaDesc.shaderPipeline = drawPipeline;

    float verts[] = {0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F};
    renderDevice->setShaderPipeline(drawPipeline);
    linePrimitive.vb = renderDevice->createVertexBuffer(sizeof(verts), verts, Usage::Dynamic);
    linePrimitive.vaDesc.buffers[0] = linePrimitive.vb;
    linePrimitive.va = renderDevice->createVertexArray(linePrimitive.vaDesc);
}

void GizmosRenderer::initBoxPrimitive()
{
    boxPrimitive.vaDesc.elementCount = 1;
    boxPrimitive.vaDesc.elements[0].name = "position";
    boxPrimitive.vaDesc.elements[0].type = Type::Float;
    boxPrimitive.vaDesc.elements[0].size = 3;
    boxPrimitive.vaDesc.elements[0].buffer.index = 0;
    boxPrimitive.vaDesc.elements[0].buffer.offset = 0;
    boxPrimitive.vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
    boxPrimitive.vaDesc.shaderPipeline = drawPipeline;

    float verts[] = {0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F,
                     0.0F, 1.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F, 1.0F, 1.0F};
    renderDevice->setShaderPipeline(drawPipeline);
    boxPrimitive.vb = renderDevice->createVertexBuffer(sizeof(verts), verts, Usage::Dynamic);
    boxPrimitive.vaDesc.buffers[0] = boxPrimitive.vb;
    boxPrimitive.va = renderDevice->createVertexArray(boxPrimitive.vaDesc);

    unsigned int indices[] = {// front
                              0, 6, 1, 0, 5, 6,
                              // right
                              1, 7, 2, 1, 6, 7,
                              // back
                              2, 7, 3, 3, 7, 4,
                              // left
                              3, 5, 0, 3, 4, 5,
                              // bottom
                              3, 1, 2, 3, 0, 1,
                              // top
                              5, 7, 6, 5, 4, 7};
    boxPrimitive.ib = renderDevice->createIndexBuffer(sizeof(indices), indices, IndexFormat::UInt, Usage::Static);
}

void GizmosRenderer::initCutConePrimitive()
{
    cutConePrimitive.vaDesc.elementCount = 1;
    cutConePrimitive.vaDesc.elements[0].name = "position";
    cutConePrimitive.vaDesc.elements[0].type = Type::Float;
    cutConePrimitive.vaDesc.elements[0].size = 3;
    cutConePrimitive.vaDesc.elements[0].buffer.index = 0;
    cutConePrimitive.vaDesc.elements[0].buffer.offset = 0;
    cutConePrimitive.vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
    cutConePrimitive.vaDesc.shaderPipeline = drawPipeline;

    float verts[CutConeVertsPerBase * 6];

    renderDevice->setShaderPipeline(drawPipeline);
    cutConePrimitive.vb = renderDevice->createVertexBuffer(sizeof(verts), verts, Usage::Dynamic);
    cutConePrimitive.vaDesc.buffers[0] = cutConePrimitive.vb;
    cutConePrimitive.va = renderDevice->createVertexArray(cutConePrimitive.vaDesc);

    unsigned int indices[12 * CutConeVertsPerBase - 12];

    int iIndex = 0;
    // Sides
    for (unsigned int i = 0; i < CutConeVertsPerBase - 1; i += 1)
    {
        indices[iIndex++] = i;
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + CutConeVertsPerBase;
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + CutConeVertsPerBase + 1;
        indices[iIndex++] = i + CutConeVertsPerBase;
    }

    // Last Side Faces
    indices[iIndex++] = CutConeVertsPerBase - 1;
    indices[iIndex++] = CutConeVertsPerBase;
    indices[iIndex++] = (CutConeVertsPerBase * 2) - 1;
    indices[iIndex++] = 0;
    indices[iIndex++] = CutConeVertsPerBase;
    indices[iIndex++] = CutConeVertsPerBase - 1;

    // Base A
    for (unsigned int i = 0; i < CutConeVertsPerBase - 2; i += 1)
    {
        indices[iIndex++] = 0;
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + 2;
    }

    // Base B
    for (unsigned int i = CutConeVertsPerBase; i < (2 * CutConeVertsPerBase) - 2; i += 1)
    {
        indices[iIndex++] = CutConeVertsPerBase;
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + 2;
    }

    cutConePrimitive.ib = renderDevice->createIndexBuffer(sizeof(indices), indices, IndexFormat::UInt, Usage::Static);
}

void GizmosRenderer::initRingPrimitive()
{
    ringPrimitive.vaDesc.elementCount = 1;
    ringPrimitive.vaDesc.elements[0].name = "position";
    ringPrimitive.vaDesc.elements[0].type = Type::Float;
    ringPrimitive.vaDesc.elements[0].size = 3;
    ringPrimitive.vaDesc.elements[0].buffer.index = 0;
    ringPrimitive.vaDesc.elements[0].buffer.offset = 0;
    ringPrimitive.vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
    ringPrimitive.vaDesc.shaderPipeline = drawPipeline;

    float verts[RingSections * 12];

    renderDevice->setShaderPipeline(drawPipeline);
    ringPrimitive.vb = renderDevice->createVertexBuffer(sizeof(verts), verts, Usage::Dynamic);
    ringPrimitive.vaDesc.buffers[0] = ringPrimitive.vb;
    ringPrimitive.va = renderDevice->createVertexArray(ringPrimitive.vaDesc);

    unsigned int indices[3 * 8 * RingSections];

    int iIndex = 0;
    // Sides
    for (unsigned int i = 0; i < (RingSections - 1) * 2; i += 2)
    {
        // TOP
        indices[iIndex++] = i;
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + 3;

        indices[iIndex++] = i;
        indices[iIndex++] = i + 3;
        indices[iIndex++] = i + 2;

        // BOTTOM
        indices[iIndex++] = i + (RingSections * 2);
        indices[iIndex++] = i + (RingSections * 2) + 1;
        indices[iIndex++] = i + (RingSections * 2) + 3;

        indices[iIndex++] = i + (RingSections * 2);
        indices[iIndex++] = i + (RingSections * 2) + 3;
        indices[iIndex++] = i + (RingSections * 2) + 2;

        // INNER
        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + (RingSections * 2) + 1;
        indices[iIndex++] = i + (RingSections * 2) + 3;

        indices[iIndex++] = i + 1;
        indices[iIndex++] = i + (RingSections * 2) + 3;
        indices[iIndex++] = i + 3;

        // OUTTER
        indices[iIndex++] = i;
        indices[iIndex++] = i + (RingSections * 2) + 2;
        indices[iIndex++] = i + 2;

        indices[iIndex++] = i;
        indices[iIndex++] = i + (RingSections * 2);
        indices[iIndex++] = i + (RingSections * 2) + 2;
    }

    // Last Side Faces
    // TOP
    indices[iIndex++] = ((RingSections - 1) * 2);
    indices[iIndex++] = ((RingSections - 1) * 2) + 1;
    indices[iIndex++] = 1;

    indices[iIndex++] = ((RingSections - 1) * 2);
    indices[iIndex++] = 1;
    indices[iIndex++] = 0;

    // BOTTOM
    indices[iIndex++] = ((RingSections - 1) * 2) + (RingSections * 2);
    indices[iIndex++] = ((RingSections - 1) * 2) + (RingSections * 2) + 1;
    indices[iIndex++] = (RingSections * 2) + 1;

    indices[iIndex++] = ((RingSections - 1) * 2) + (RingSections * 2);
    indices[iIndex++] = (RingSections * 2) + 1;
    indices[iIndex++] = (RingSections * 2);

    // INNER
    indices[iIndex++] = ((RingSections - 1) * 2) + 1;
    indices[iIndex++] = ((RingSections - 1) * 2) + (RingSections * 2) + 1;
    indices[iIndex++] = (RingSections * 2) + 1;

    indices[iIndex++] = ((RingSections - 1) * 2) + 1;
    indices[iIndex++] = (RingSections * 2) + 1;
    indices[iIndex++] = 1;

    // OUTTER
    indices[iIndex++] = ((RingSections - 1) * 2);
    indices[iIndex++] = (RingSections * 2);
    indices[iIndex++] = 0;

    indices[iIndex++] = ((RingSections - 1) * 2);
    indices[iIndex++] = ((RingSections - 1) * 2) + (RingSections * 2);
    indices[iIndex++] = (RingSections * 2) + 0;

    ringPrimitive.ib = renderDevice->createIndexBuffer(sizeof(indices), indices, IndexFormat::UInt, Usage::Static);
}
