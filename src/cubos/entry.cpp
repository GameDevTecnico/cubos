#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    // Scope to destroy objects before the window is deleted
    {
        auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
            #version 330 core

            in vec2 position;
            in vec2 uv;

            out vec2 fragUV;

            uniform MVP
            {
                mat4 mvp;
            };

            void main()
            {
                fragUV = uv;
                gl_Position = mvp * vec4(position, 0.0f, 1.0f);
            }
        )");

        auto ps = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 330 core

            in vec2 fragUV;
            out vec4 color;

            uniform sampler2D colorTexture;

            void main()
            {
                color = vec4(texture(colorTexture, fragUV).rgb, 1.0f);
            }
        )");

        auto pp = renderDevice.createShaderPipeline(vs, ps);

        float verts[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, +0.5f, 0.0f, 1.0f, +0.5f, +0.5f, 1.0f, 1.0f, +0.5f, -0.5f, 1.0f, 0.0f,
        };

        auto vb = renderDevice.createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
        };

        auto ib = renderDevice.createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

        auto cb = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
        auto cbBP = pp->getBindingPoint("MVP");

        float textureData[] = {
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        };

        gl::Texture2DDesc textureDesc;
        textureDesc.data[0] = textureData;
        textureDesc.format = gl::TextureFormat::RGB32Float;
        textureDesc.width = 2;
        textureDesc.height = 2;
        auto texture = renderDevice.createTexture2D(textureDesc);
        auto textureBP = pp->getBindingPoint("colorTexture");

        gl::SamplerDesc samplerDesc;
        samplerDesc.minFilter = gl::TextureFilter::Linear;
        samplerDesc.magFilter = gl::TextureFilter::Linear;
        samplerDesc.addressU = gl::AddressMode::Mirror;
        samplerDesc.addressV = gl::AddressMode::Mirror;
        auto sampler = renderDevice.createSampler(samplerDesc);

        gl::VertexArrayDesc vaDesc;
        vaDesc.elementCount = 2;
        vaDesc.elements[0].name = "position";
        vaDesc.elements[0].type = gl::Type::Float;
        vaDesc.elements[0].size = 2;
        vaDesc.elements[0].buffer.index = 0;
        vaDesc.elements[0].buffer.offset = 0;
        vaDesc.elements[0].buffer.stride = 4 * sizeof(float);
        vaDesc.elements[1].name = "uv";
        vaDesc.elements[1].type = gl::Type::Float;
        vaDesc.elements[1].size = 2;
        vaDesc.elements[1].buffer.index = 0;
        vaDesc.elements[1].buffer.offset = 2 * sizeof(float);
        vaDesc.elements[1].buffer.stride = 4 * sizeof(float);
        vaDesc.buffers[0] = vb;
        vaDesc.shaderPipeline = pp;
        auto va = renderDevice.createVertexArray(vaDesc);

        gl::RasterStateDesc rsDesc;
        rsDesc.rasterMode = gl::RasterMode::Wireframe;
        auto rs = renderDevice.createRasterState(rsDesc);

        float t = 0.0f;

        while (!window->shouldClose())
        {
            auto sz = window->getFramebufferSize();
            renderDevice.setViewport(0, 0, sz.x, sz.y);

            renderDevice.clearColor(0.894f, 0.592f, 0.141f, 0.0f);

            renderDevice.setShaderPipeline(pp);
            renderDevice.setVertexArray(va);
            renderDevice.setIndexBuffer(ib);

            textureBP->bind(texture);
            textureBP->bind(sampler);
            cbBP->bind(cb);

            {
                auto& mvp = *(glm::mat4*)cb->map();
                mvp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
                      glm::lookAt(glm::vec3{0.0f, 0.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                                  glm::vec3{0.0f, 1.0f, 0.0f}) *
                      glm::translate(glm::mat4(1.0f), glm::vec3{-2.0f, 0.0f, 0.0f}) *
                      glm::rotate(glm::mat4(1.0f), t, glm::vec3{0.0f, 1.0f, 0.0f});
                cb->unmap();
            }

            renderDevice.setRasterState(nullptr);
            renderDevice.drawTrianglesIndexed(0, 6);

            {
                auto& mvp = *(glm::mat4*)cb->map();
                mvp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
                      glm::lookAt(glm::vec3{0.0f, 0.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                                  glm::vec3{0.0f, 1.0f, 0.0f}) *
                      glm::translate(glm::mat4(1.0f), glm::vec3{2.0f, 0.0f, 0.0f}) *
                      glm::rotate(glm::mat4(1.0f), t, glm::vec3{0.0f, 1.0f, 0.0f});
                cb->unmap();
            }

            renderDevice.setRasterState(rs);
            renderDevice.drawTrianglesIndexed(0, 6);

            window->swapBuffers();
            window->pollEvents();

            t += 0.01f;
        }
    }

    delete window;

    return 0;
}
