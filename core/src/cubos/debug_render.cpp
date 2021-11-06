#include <cubos/debug_render.hpp>

#include <list>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::gl;

RenderDevice* Debug::renderDevice;
ConstantBuffer Debug::mvpBuffer;
ShaderBindingPoint Debug::mvpBindingPoint, Debug::colorBindingPoint;
ShaderPipeline Debug::pipeline;
RasterState Debug::fillRasterState, Debug::wireframeRasterState;
Debug::DebugDrawObject Debug::objCube, Debug::objSphere;
std::list<Debug::DebugDrawRequest> Debug::requests;
std::mutex Debug::debugDrawMutex;

void Debug::initCube()
{
    float verts[] = {// front
                     -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
                     // back
                     -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

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
    float verts[] = {0.0f,
                     -1.0f,
                     0.0f,
                     0.0f,
                     1.0f,
                     0.0f,
                     0.43388372659683f,
                     -0.90096884965897f,
                     0.0f,
                     0.39091575145721f,
                     -0.90096884965897f,
                     0.18825510144234f,
                     0.27052208781242f,
                     -0.90096884965897f,
                     0.33922398090363f,
                     0.09654821455479f,
                     -0.90096884965897f,
                     0.42300537228584f,
                     -0.09654821455479f,
                     -0.90096884965897f,
                     0.42300537228584f,
                     -0.27052208781242f,
                     -0.90096884965897f,
                     0.33922398090363f,
                     -0.39091575145721f,
                     -0.90096884965897f,
                     0.18825510144234f,
                     -0.43388372659683f,
                     -0.90096884965897f,
                     0.0f,
                     -0.39091575145721f,
                     -0.90096884965897f,
                     -0.18825510144234f,
                     -0.27052208781242f,
                     -0.90096884965897f,
                     -0.33922398090363f,
                     -0.09654821455479f,
                     -0.90096884965897f,
                     -0.42300537228584f,
                     0.09654821455479f,
                     -0.90096884965897f,
                     -0.42300537228584f,
                     0.27052208781242f,
                     -0.90096884965897f,
                     -0.33922398090363f,
                     0.39091575145721f,
                     -0.90096884965897f,
                     -0.18825510144234f,
                     0.78183150291443f,
                     -0.62348979711533f,
                     0.0f,
                     0.70440584421158f,
                     -0.62348979711533f,
                     0.33922398090363f,
                     0.48746395111084f,
                     -0.62348979711533f,
                     0.61126047372818f,
                     0.17397387325764f,
                     -0.62348979711533f,
                     0.76222932338715f,
                     -0.17397387325764f,
                     -0.62348979711533f,
                     0.76222932338715f,
                     -0.48746395111084f,
                     -0.62348979711533f,
                     0.61126047372818f,
                     -0.70440584421158f,
                     -0.62348979711533f,
                     0.33922398090363f,
                     -0.78183150291443f,
                     -0.62348979711533f,
                     0.0f,
                     -0.70440584421158f,
                     -0.62348979711533f,
                     -0.33922398090363f,
                     -0.48746395111084f,
                     -0.62348979711533f,
                     -0.61126047372818f,
                     -0.17397387325764f,
                     -0.62348979711533f,
                     -0.76222932338715f,
                     0.17397387325764f,
                     -0.62348979711533f,
                     -0.76222932338715f,
                     0.48746395111084f,
                     -0.62348979711533f,
                     -0.61126047372818f,
                     0.70440584421158f,
                     -0.62348979711533f,
                     -0.33922398090363f,
                     0.97492790222168f,
                     -0.2225209325552f,
                     0.0f,
                     0.87837970256805f,
                     -0.2225209325552f,
                     0.42300537228584f,
                     0.60785758495331f,
                     -0.2225209325552f,
                     0.76222932338715f,
                     0.21694186329842f,
                     -0.2225209325552f,
                     0.95048445463181f,
                     -0.21694186329842f,
                     -0.2225209325552f,
                     0.95048445463181f,
                     -0.60785758495331f,
                     -0.2225209325552f,
                     0.76222932338715f,
                     -0.87837970256805f,
                     -0.2225209325552f,
                     0.42300537228584f,
                     -0.97492790222168f,
                     -0.2225209325552f,
                     0.0f,
                     -0.87837970256805f,
                     -0.2225209325552f,
                     -0.42300537228584f,
                     -0.60785758495331f,
                     -0.2225209325552f,
                     -0.76222932338715f,
                     -0.21694186329842f,
                     -0.2225209325552f,
                     -0.95048445463181f,
                     0.21694186329842f,
                     -0.2225209325552f,
                     -0.95048445463181f,
                     0.60785758495331f,
                     -0.2225209325552f,
                     -0.76222932338715f,
                     0.87837970256805f,
                     -0.2225209325552f,
                     -0.42300537228584f,
                     0.97492790222168f,
                     0.2225209325552f,
                     0.0f,
                     0.87837970256805f,
                     0.2225209325552f,
                     0.42300537228584f,
                     0.60785758495331f,
                     0.2225209325552f,
                     0.76222932338715f,
                     0.21694186329842f,
                     0.2225209325552f,
                     0.95048445463181f,
                     -0.21694186329842f,
                     0.2225209325552f,
                     0.95048445463181f,
                     -0.60785758495331f,
                     0.2225209325552f,
                     0.76222932338715f,
                     -0.87837970256805f,
                     0.2225209325552f,
                     0.42300537228584f,
                     -0.97492790222168f,
                     0.2225209325552f,
                     0.0f,
                     -0.87837970256805f,
                     0.2225209325552f,
                     -0.42300537228584f,
                     -0.60785758495331f,
                     0.2225209325552f,
                     -0.76222932338715f,
                     -0.21694186329842f,
                     0.2225209325552f,
                     -0.95048445463181f,
                     0.21694186329842f,
                     0.2225209325552f,
                     -0.95048445463181f,
                     0.60785758495331f,
                     0.2225209325552f,
                     -0.76222932338715f,
                     0.87837970256805f,
                     0.2225209325552f,
                     -0.42300537228584f,
                     0.78183150291443f,
                     0.62348979711533f,
                     0.0f,
                     0.70440584421158f,
                     0.62348979711533f,
                     0.33922398090363f,
                     0.48746395111084f,
                     0.62348979711533f,
                     0.61126047372818f,
                     0.17397387325764f,
                     0.62348979711533f,
                     0.76222932338715f,
                     -0.17397387325764f,
                     0.62348979711533f,
                     0.76222932338715f,
                     -0.48746395111084f,
                     0.62348979711533f,
                     0.61126047372818f,
                     -0.70440584421158f,
                     0.62348979711533f,
                     0.33922398090363f,
                     -0.78183150291443f,
                     0.62348979711533f,
                     0.0f,
                     -0.70440584421158f,
                     0.62348979711533f,
                     -0.33922398090363f,
                     -0.48746395111084f,
                     0.62348979711533f,
                     -0.61126047372818f,
                     -0.17397387325764f,
                     0.62348979711533f,
                     -0.76222932338715f,
                     0.17397387325764f,
                     0.62348979711533f,
                     -0.76222932338715f,
                     0.48746395111084f,
                     0.62348979711533f,
                     -0.61126047372818f,
                     0.70440584421158f,
                     0.62348979711533f,
                     -0.33922398090363f,
                     0.43388372659683f,
                     0.90096884965897f,
                     0.0f,
                     0.39091575145721f,
                     0.90096884965897f,
                     0.18825510144234f,
                     0.27052208781242f,
                     0.90096884965897f,
                     0.33922398090363f,
                     0.09654821455479f,
                     0.90096884965897f,
                     0.42300537228584f,
                     -0.09654821455479f,
                     0.90096884965897f,
                     0.42300537228584f,
                     -0.27052208781242f,
                     0.90096884965897f,
                     0.33922398090363f,
                     -0.39091575145721f,
                     0.90096884965897f,
                     0.18825510144234f,
                     -0.43388372659683f,
                     0.90096884965897f,
                     0.0f,
                     -0.39091575145721f,
                     0.90096884965897f,
                     -0.18825510144234f,
                     -0.27052208781242f,
                     0.90096884965897f,
                     -0.33922398090363f,
                     -0.09654821455479f,
                     0.90096884965897f,
                     -0.42300537228584f,
                     0.09654821455479f,
                     0.90096884965897f,
                     -0.42300537228584f,
                     0.27052208781242f,
                     0.90096884965897f,
                     -0.33922398090363f,
                     0.39091575145721f,
                     0.90096884965897f,
                     -0.18825510144234f};

    auto vb = renderDevice->createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

    unsigned int indices[] = {
        2,  16, 17, 3,  17, 18, 4,  18, 19, 5,  19, 20, 6,  20, 21, 7,  21, 22, 8,  22, 23, 9,  23, 24, 10, 24, 25, 11,
        25, 26, 12, 26, 27, 13, 27, 28, 14, 28, 29, 15, 29, 16, 16, 30, 31, 17, 31, 32, 18, 32, 33, 19, 33, 34, 20, 34,
        35, 21, 35, 36, 22, 36, 37, 23, 37, 38, 24, 38, 39, 25, 39, 40, 26, 40, 41, 27, 41, 42, 28, 42, 43, 29, 43, 30,
        30, 44, 45, 31, 45, 46, 32, 46, 47, 33, 47, 48, 34, 48, 49, 35, 49, 50, 36, 50, 51, 37, 51, 52, 38, 52, 53, 39,
        53, 54, 40, 54, 55, 41, 55, 56, 42, 56, 57, 43, 57, 44, 44, 58, 59, 45, 59, 60, 46, 60, 61, 47, 61, 62, 48, 62,
        63, 49, 63, 64, 50, 64, 65, 51, 65, 66, 52, 66, 67, 53, 67, 68, 54, 68, 69, 55, 69, 70, 56, 70, 71, 57, 71, 58,
        58, 72, 73, 59, 73, 74, 60, 74, 75, 61, 75, 76, 62, 76, 77, 63, 77, 78, 64, 78, 79, 65, 79, 80, 66, 80, 81, 67,
        81, 82, 68, 82, 83, 69, 83, 84, 70, 84, 85, 71, 85, 72, 0,  2,  3,  73, 72, 1,  0,  3,  4,  74, 73, 1,  0,  4,
        5,  75, 74, 1,  0,  5,  6,  76, 75, 1,  0,  6,  7,  77, 76, 1,  0,  7,  8,  78, 77, 1,  0,  8,  9,  79, 78, 1,
        0,  9,  10, 80, 79, 1,  0,  10, 11, 81, 80, 1,  0,  11, 12, 82, 81, 1,  0,  12, 13, 83, 82, 1,  0,  13, 14, 84,
        83, 1,  0,  14, 15, 85, 84, 1,  0,  15, 2,  72, 85, 1,  2,  17, 3,  3,  18, 4,  4,  19, 5,  5,  20, 6,  6,  21,
        7,  7,  22, 8,  8,  23, 9,  9,  24, 10, 10, 25, 11, 11, 26, 12, 12, 27, 13, 13, 28, 14, 14, 29, 15, 15, 16, 2,
        16, 31, 17, 17, 32, 18, 18, 33, 19, 19, 34, 20, 20, 35, 21, 21, 36, 22, 22, 37, 23, 23, 38, 24, 24, 39, 25, 25,
        40, 26, 26, 41, 27, 27, 42, 28, 28, 43, 29, 29, 30, 16, 30, 45, 31, 31, 46, 32, 32, 47, 33, 33, 48, 34, 34, 49,
        35, 35, 50, 36, 36, 51, 37, 37, 52, 38, 38, 53, 39, 39, 54, 40, 40, 55, 41, 41, 56, 42, 42, 57, 43, 43, 44, 30,
        44, 59, 45, 45, 60, 46, 46, 61, 47, 47, 62, 48, 48, 63, 49, 49, 64, 50, 50, 65, 51, 51, 66, 52, 52, 67, 53, 53,
        68, 54, 54, 69, 55, 55, 70, 56, 56, 71, 57, 57, 58, 44, 58, 73, 59, 59, 74, 60, 60, 75, 61, 61, 76, 62, 62, 77,
        63, 63, 78, 64, 64, 79, 65, 65, 80, 66, 66, 81, 67, 67, 82, 68, 68, 83, 69, 69, 84, 70, 70, 85, 71, 71, 72, 58};

    objSphere.numIndices = sizeof(indices) / sizeof(unsigned int);
    objSphere.ib = renderDevice->createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

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

void Debug::init(gl::RenderDevice& targetRenderDevice)
{
    renderDevice = &targetRenderDevice;

    auto vs = renderDevice->createShaderStage(gl::Stage::Vertex, R"(
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

    auto ps = renderDevice->createShaderStage(gl::Stage::Pixel, R"(
            #version 330 core

            out vec4 color;

            uniform vec3 objColor;

            void main()
            {
                color = vec4(objColor, 1.0f);
            }
        )");

    pipeline = renderDevice->createShaderPipeline(vs, ps);

    initCube();
    initSphere();

    mvpBuffer = renderDevice->createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    mvpBindingPoint = pipeline->getBindingPoint("MVP");
    colorBindingPoint = pipeline->getBindingPoint("objColor");

    RasterStateDesc rsDesc;

    rsDesc.rasterMode = RasterMode::Fill;
    fillRasterState = renderDevice->createRasterState(rsDesc);

    rsDesc.rasterMode = RasterMode::Wireframe;
    wireframeRasterState = renderDevice->createRasterState(rsDesc);
}

void Debug::drawCube(glm::vec3 center, glm::vec3 size, float time, glm::quat rotation, glm::vec3 color)
{
    debugDrawMutex.lock();
    requests.push_back(DebugDrawRequest{
        objCube, fillRasterState, glm::translate(center) * glm::toMat4(rotation) * glm::scale(size), time, color});
    debugDrawMutex.unlock();
}

void Debug::drawWireCube(glm::vec3 center, glm::vec3 size, float time, glm::quat rotation, glm::vec3 color)
{
    debugDrawMutex.lock();
    requests.push_back(DebugDrawRequest{
        objCube, wireframeRasterState, glm::translate(center) * glm::toMat4(rotation) * glm::scale(size), time, color});
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
    renderDevice->setRasterState(nullptr);
}
