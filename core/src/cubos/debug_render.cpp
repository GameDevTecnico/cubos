#include <cubos/debug_render.hpp>

#include <cubos/gl/render_device.hpp>

#include <list>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace cubos;
using namespace cubos::gl;

static RenderDevice* debugRenderDevice;
static ConstantBuffer mvpBuffer;
static ShaderBindingPoint mvpBindingPoint;
static ShaderPipeline pipeline;
static io::Window* debugWindow;

static RasterState FillRasterState, WireframeRasterState;

struct DebugDrawObject
{
    VertexArray va;
    IndexBuffer ib;
};

static DebugDrawObject objCube, objSphere;

struct DebugDrawRequest
{
    DebugDrawObject obj;
    RasterState rasterState;
    glm::mat4 modelMatrix;
    float timeLeft;
};

static std::list<DebugDrawRequest> requests;

static void initCube()
{
    float verts[] = {
        0.5f,  0.5f, 0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,  -0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f,
    };

    auto vb = debugRenderDevice->createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
    };

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

            uniform vec3 color;

            void main()
            {
                color = vec4(color, 1.0f);
            }
        )");

    pipeline = debugRenderDevice->createShaderPipeline(vs, ps);

    initCube();

    mvpBuffer = debugRenderDevice->createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    mvpBindingPoint = pipeline->getBindingPoint("MVP");

    RasterStateDesc rsDesc;

    rsDesc.rasterMode = RasterMode::Fill;
    FillRasterState = debugRenderDevice->createRasterState(rsDesc);

    rsDesc.rasterMode = RasterMode::Wireframe;
    WireframeRasterState = debugRenderDevice->createRasterState(rsDesc);
}

void debug::drawCube(glm::mat4 vp, glm::vec3 center, glm::vec3 size, float time)
{
    requests.push_back(DebugDrawRequest{objCube, FillRasterState, glm::scale(size), time});
}

void debug::drawWireCube(glm::mat4 vp, glm::vec3 center, glm::vec3 size, float time)
{
    requests.push_back(DebugDrawRequest{objCube, WireframeRasterState, glm::scale(size), time});
}

void debug::drawSphere(glm::mat4 vp, glm::vec3 center, float radius, float time)
{
    requests.push_back(DebugDrawRequest{objSphere, FillRasterState, glm::scale(glm::vec3(radius)), time});
}

void debug::drawWireSphere(glm::mat4 vp, glm::vec3 center, float radius, float time)
{
    requests.push_back(DebugDrawRequest{objSphere, WireframeRasterState, glm::scale(glm::vec3(radius)), time});
}

void debug::flush(float deltaT)
{
    debugRenderDevice->setShaderPipeline(pipeline);
    auto sz = debugWindow->getFramebufferSize();
    for (auto it = requests.begin(); it != requests.end();)
    {
        debugRenderDevice->setVertexArray(it->obj.va);
        debugRenderDevice->setIndexBuffer(it->obj.ib);
        mvpBindingPoint->bind(mvpBuffer);

        auto& mvp = *(glm::mat4*)mvpBuffer->map();
        mvp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
              glm::lookAt(glm::vec3{0.0f, 0.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}) *
              it->modelMatrix;
        mvpBuffer->unmap();

        debugRenderDevice->setRasterState(it->rasterState);
        debugRenderDevice->drawTrianglesIndexed(0, 6);
        
        it->timeLeft -= deltaT;

        auto current = it++;
        if (it->timeLeft <= 0) {
            requests.erase(current);
        }
    }
}
