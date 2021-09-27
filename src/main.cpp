#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

using namespace cubos;

int main(void)
{
    auto window = io::Window();
    auto& renderDevice = window.getRenderDevice();

    auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
        #version 330 core

        in vec2 position;
        in vec3 color;

        uniform MVP
        {
            mat4 mvp;
        };

        out vec3 fragColor;

        void main()
        {
            fragColor = color;
            gl_Position = mvp * vec4(position, 0.0f, 1.0f);
        }
    )");

    auto ps = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
        #version 330 core

        in vec3 fragColor;
        out vec4 color;

        void main()
        {
            color = vec4(fragColor, 1.0f);
        }
    )");

    auto pp = renderDevice.createShaderPipeline(vs, ps);

    float verts[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.5f, +0.5f, 0.0f, 1.0f, 0.0f,
        +0.5f, +0.5f, 0.0f, 0.0f, 1.0f, +0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    };

    auto vb = renderDevice.createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
    };

    auto ib = renderDevice.createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

    auto cb = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    auto cbBP = pp->getBindingPoint("MVP");

    gl::VertexArrayDesc vaDesc;
    vaDesc.elementCount = 2;
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = gl::Type::Float;
    vaDesc.elements[0].size = 2;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = 0;
    vaDesc.elements[0].buffer.stride = 5 * sizeof(float);
    vaDesc.elements[1].name = "color";
    vaDesc.elements[1].type = gl::Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 0;
    vaDesc.elements[1].buffer.offset = 2 * sizeof(float);
    vaDesc.elements[1].buffer.stride = 5 * sizeof(float);
    vaDesc.buffers[0] = vb;
    vaDesc.shaderPipeline = pp;
    auto va = renderDevice.createVertexArray(vaDesc);

    float t = 0.0f;

    while (!window.shouldClose())
    {
        auto sz = window.getFramebufferSize();
        renderDevice.setViewport(0, 0, sz.x, sz.y);

        renderDevice.clearColor(0.894f, 0.592f, 0.141f, 0.0f);

        renderDevice.setShaderPipeline(pp);
        renderDevice.setVertexArray(va);
        renderDevice.setIndexBuffer(ib);

        auto& mvp = *(glm::mat4*)cb->map();
        mvp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
              glm::lookAt(glm::vec3{0.0f, 0.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}) *
              glm::rotate(glm::mat4(1.0f), t, glm::vec3{ 0.0f, 1.0f, 0.0f });
        cb->unmap();
        cbBP->bind(cb);
        renderDevice.drawTrianglesIndexed(0, 6);

        window.swapBuffers();
        window.pollEvents();

        t += 0.01f;
    }

    return 0;
}
