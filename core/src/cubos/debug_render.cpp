#include <cubos/debug_render.hpp>

#include <cubos/gl/render_device.hpp>

#include <list>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace cubos;
using namespace cubos::gl;

static RenderDevice* debugRenderDevice;
static ConstantBuffer mvpBuffer;
static ShaderBindingPoint mvpBindingPoint, colorBindingPoint;
static ShaderPipeline pipeline;
static io::Window* debugWindow;

static RasterState FillRasterState, WireframeRasterState;

struct DebugDrawObject
{
    VertexArray va;
    IndexBuffer ib;
    int numIndices;
};

static DebugDrawObject objCube, objSphere;

struct DebugDrawRequest
{
    DebugDrawObject obj;
    RasterState rasterState;
    glm::mat4 modelMatrix;
    double timeLeft;
    glm::vec3 color;
};

static std::list<DebugDrawRequest> requests;

static void initCube()
{
    float verts[] = {// front
                     -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
                     // back
                     -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0};

    auto vb = debugRenderDevice->createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

    unsigned int indices[] = {// front
                              0, 1, 2, 2, 3, 0,
                              // right
                              1, 5, 6, 6, 2, 1,
                              // back
                              7, 6, 5, 5, 4, 7,
                              // left
                              4, 0, 3, 3, 7, 4,
                              // bottom
                              4, 5, 1, 1, 0, 4,
                              // top
                              3, 2, 6, 6, 7, 3};

    objCube.numIndices = sizeof(indices) / sizeof(unsigned int);
    objCube.ib =
        debugRenderDevice->createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

    gl::VertexArrayDesc vaDesc;
    vaDesc.elementCount = 1;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = gl::Type::Float;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = 0;
    vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
    vaDesc.buffers[0] = vb;
    vaDesc.shaderPipeline = pipeline;
    objCube.va = debugRenderDevice->createVertexArray(vaDesc);
}

void debug::init(io::Window* window)
{
    debugWindow = window;
    debugRenderDevice = &debugWindow->getRenderDevice();

    auto vs = debugRenderDevice->createShaderStage(gl::Stage::Vertex, R"(
            #version 330 core

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

    auto ps = debugRenderDevice->createShaderStage(gl::Stage::Pixel, R"(
            #version 330 core

            out vec4 color;

            uniform vec3 objColor;

            void main()
            {
                color = vec4(objColor, 1.0f);
            }
        )");

    pipeline = debugRenderDevice->createShaderPipeline(vs, ps);

    initCube();

    mvpBuffer = debugRenderDevice->createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    mvpBindingPoint = pipeline->getBindingPoint("MVP");
    colorBindingPoint = pipeline->getBindingPoint("objColor");

    RasterStateDesc rsDesc;

    rsDesc.rasterMode = RasterMode::Fill;
    FillRasterState = debugRenderDevice->createRasterState(rsDesc);

    rsDesc.rasterMode = RasterMode::Wireframe;
    WireframeRasterState = debugRenderDevice->createRasterState(rsDesc);
}

void debug::drawCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color)
{
    requests.push_back(
        DebugDrawRequest{objCube, FillRasterState, glm::translate(center) * glm::scale(size), time, color});
}

void debug::drawWireCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color)
{
    requests.push_back(
        DebugDrawRequest{objCube, WireframeRasterState, glm::translate(center) * glm::scale(size), time, color});
}

void debug::drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color)
{
    requests.push_back(DebugDrawRequest{objSphere, FillRasterState,
                                        glm::translate(center) * glm::scale(glm::vec3(radius)), time, color});
}

void debug::drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color)
{
    requests.push_back(DebugDrawRequest{objSphere, WireframeRasterState,
                                        glm::translate(center) * glm::scale(glm::vec3(radius)), time, color});
}

void debug::flush(glm::mat4 vp, double deltaT)
{
    debugRenderDevice->setShaderPipeline(pipeline);
    auto sz = debugWindow->getFramebufferSize();
    for (auto it = requests.begin(); it != requests.end();)
    {
        debugRenderDevice->setVertexArray(it->obj.va);
        debugRenderDevice->setIndexBuffer(it->obj.ib);
        mvpBindingPoint->bind(mvpBuffer);

        auto& mvp = *(glm::mat4*)mvpBuffer->map();
        mvp = vp * it->modelMatrix;
        mvpBuffer->unmap();

        colorBindingPoint->setConstant(it->color);

        debugRenderDevice->setRasterState(it->rasterState);
        debugRenderDevice->drawTrianglesIndexed(0, it->obj.numIndices);

        it->timeLeft -= deltaT;

        auto current = it++;
        if (current->timeLeft <= 0)
        {
            requests.erase(current);
        }
    }
    debugRenderDevice->setRasterState(nullptr);
}
