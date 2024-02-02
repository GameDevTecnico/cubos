#include <array>
#include <list>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <cubos/core/gl/debug.hpp>

using namespace cubos::core;
using namespace cubos::core::gl;

RenderDevice* Debug::renderDevice;
ConstantBuffer Debug::mvpBuffer;
ShaderBindingPoint Debug::mvpBindingPoint, Debug::colorBindingPoint;
ShaderPipeline Debug::pipeline;
RasterState Debug::fillRasterState, Debug::wireframeRasterState;
Debug::DebugDrawObject Debug::objCube, Debug::objSphere;
std::list<Debug::DebugDrawRequest> Debug::requests;
std::mutex Debug::debugDrawMutex;

void Debug::DebugDrawObject::clear()
{
    va = nullptr;
    ib = nullptr;
}

void Debug::initCube()
{
    float verts[] = {// front
                     -0.5F, -0.5F, 0.5F, 0.5F, -0.5F, 0.5F, 0.5F, 0.5F, 0.5F, -0.5F, 0.5F, 0.5F,
                     // back
                     -0.5F, -0.5F, -0.5F, 0.5F, -0.5F, -0.5F, 0.5F, 0.5F, -0.5F, -0.5F, 0.5F, -0.5F};

    auto vb = renderDevice->createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

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
    objCube.ib = renderDevice->createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

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
    objCube.va = renderDevice->createVertexArray(vaDesc);
}

void Debug::initSphere()
{
    unsigned int sectorCount = 10;
    unsigned int stackCount = 10;

    std::vector<float> vertices;

    float x;
    float y;
    float z;
    float xz;

    auto sectorStep = (float)(2 * glm::pi<float>() / static_cast<float>(sectorCount));
    auto stackStep = (float)(glm::pi<float>() / static_cast<float>(stackCount));
    float sectorAngle;
    float stackAngle;

    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        stackAngle = glm::pi<float>() / 2.0F - static_cast<float>(i) * stackStep;
        xz = glm::cos(stackAngle);
        y = glm::sin(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = static_cast<float>(j) * sectorStep;

            x = xz * glm::cos(sectorAngle);
            z = xz * glm::sin(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    auto vb = renderDevice->createVertexBuffer(vertices.size() * sizeof(float), vertices.data(), gl::Usage::Static);

    std::vector<unsigned int> indices;
    unsigned int k1;
    unsigned int k2;
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    objSphere.numIndices = (unsigned int)indices.size();
    objSphere.ib = renderDevice->createIndexBuffer(indices.size() * sizeof(unsigned int), indices.data(),
                                                   gl::IndexFormat::UInt, gl::Usage::Static);

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
    objSphere.va = renderDevice->createVertexArray(vaDesc);
}

void Debug::init(RenderDevice& renderDevice)
{
    Debug::renderDevice = &renderDevice;

    auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
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

    auto ps = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 330 core

            out vec4 color;

            uniform vec3 objColor;

            void main()
            {
                color = vec4(objColor, 1.0f);
            }
        )");

    pipeline = renderDevice.createShaderPipeline(vs, ps);

    initCube();
    initSphere();

    mvpBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    mvpBindingPoint = pipeline->getBindingPoint("MVP");
    colorBindingPoint = pipeline->getBindingPoint("objColor");

    RasterStateDesc rsDesc;
    rsDesc.rasterMode = RasterMode::Fill;
    fillRasterState = renderDevice.createRasterState(rsDesc);
    rsDesc.rasterMode = RasterMode::Wireframe;
    wireframeRasterState = renderDevice.createRasterState(rsDesc);
}

void Debug::drawLine(glm::vec3 start, glm::vec3 end, bool relative, glm::vec3 color, float time)
{
    auto vec = relative ? end : end - start;
    auto transform = glm::translate(start) * glm::translate(vec / 2.0F) *
                     glm::orientation(glm::normalize(vec), glm::vec3{0.0F, 1.0F, 0.0F}) *
                     glm::scale(glm::vec3{0.1F, glm::length(vec), 0.1F});
    debugDrawMutex.lock();
    requests.push_back(DebugDrawRequest{objCube, fillRasterState, transform, time, color});
    debugDrawMutex.unlock();
}

void Debug::drawBox(geom::Box box, glm::mat4 transform, glm::vec3 color, float time)
{
    debugDrawMutex.lock();
    requests.push_back(
        DebugDrawRequest{objCube, fillRasterState, transform * glm::scale(2.0F * box.halfSize), time, color});
    debugDrawMutex.unlock();
}

void Debug::drawWireBox(geom::Box box, glm::mat4 transform, glm::vec3 color, float time)
{
    debugDrawMutex.lock();
    requests.push_back(
        DebugDrawRequest{objCube, wireframeRasterState, transform * glm::scale(2.0F * box.halfSize), time, color});
    debugDrawMutex.unlock();
}

void Debug::drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color)
{
    debugDrawMutex.lock();
    requests.push_back(DebugDrawRequest{objSphere, fillRasterState,
                                        glm::translate(center) * glm::scale(glm::vec3(radius)), time, color});
    debugDrawMutex.unlock();
}

void Debug::drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color)
{
    debugDrawMutex.lock();
    requests.push_back(DebugDrawRequest{objSphere, wireframeRasterState,
                                        glm::translate(center) * glm::scale(glm::vec3(radius)), time, color});
    debugDrawMutex.unlock();
}

void Debug::flush(glm::mat4 vp, double deltaT)
{
    renderDevice->setShaderPipeline(pipeline);

    debugDrawMutex.lock();
    for (auto it = requests.begin(); it != requests.end();)
    {
        renderDevice->setVertexArray(it->obj.va);
        renderDevice->setIndexBuffer(it->obj.ib);
        mvpBindingPoint->bind(mvpBuffer);

        auto& mvp = *(glm::mat4*)mvpBuffer->map();
        mvp = vp * it->modelMatrix;
        mvpBuffer->unmap();

        colorBindingPoint->setConstant(it->color);

        renderDevice->setRasterState(it->rasterState);
        renderDevice->drawTrianglesIndexed(0, it->obj.numIndices);

        it->timeLeft -= deltaT;

        auto current = it++;
        if (current->timeLeft <= 0)
        {
            requests.erase(current);
        }
    }
    debugDrawMutex.unlock();
}

void Debug::terminate()
{
    mvpBuffer = nullptr;
    mvpBindingPoint = colorBindingPoint = nullptr;
    pipeline = nullptr;
    fillRasterState = wireframeRasterState = nullptr;
    objCube.clear();
    objSphere.clear();
    requests.clear();
}
