#include <cstdio>

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core;

int main()
{
    auto window = io::openWindow();
    auto& renderDevice = window->renderDevice();

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
            -0.5F, -0.5F, 0.0F, 0.0F, -0.5F, +0.5F, 0.0F, 1.0F, +0.5F, +0.5F, 1.0F, 1.0F, +0.5F, -0.5F, 1.0F, 0.0F,
        };

        auto vb = renderDevice.createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
        };

        auto ib = renderDevice.createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

        auto cb = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
        auto* cbBP = pp->getBindingPoint("MVP");

        float textureData[] = {
            0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F,
        };

        gl::Texture2DDesc textureDesc;
        textureDesc.data[0] = textureData;
        textureDesc.format = gl::TextureFormat::RGB32Float;
        textureDesc.width = 2;
        textureDesc.height = 2;
        auto texture = renderDevice.createTexture2D(textureDesc);
        auto* textureBP = pp->getBindingPoint("colorTexture");

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
        auto rs = renderDevice.createRasterState(rsDesc);

        float t = 0.0F;

        while (!window->shouldClose())
        {
            auto sz = window->framebufferSize();
            renderDevice.setViewport(0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y));

            renderDevice.clearColor(0.894F, 0.592F, 0.141F, 0.0F);

            renderDevice.setShaderPipeline(pp);
            renderDevice.setVertexArray(va);
            renderDevice.setIndexBuffer(ib);

            textureBP->bind(texture);
            textureBP->bind(sampler);
            cbBP->bind(cb);

            {
                auto& mvp = *(glm::mat4*)cb->map();
                mvp = glm::perspective(glm::radians(70.0F), float(sz.x) / float(sz.y), 0.1F, 1000.0F) *
                      glm::lookAt(glm::vec3{0.0F, 0.0F, -5.0F}, glm::vec3{0.0F, 0.0F, 0.0F},
                                  glm::vec3{0.0F, 1.0F, 0.0F}) *
                      glm::translate(glm::mat4(1.0F), glm::vec3{-2.0F, 0.0F, 0.0F}) *
                      glm::rotate(glm::mat4(1.0F), t, glm::vec3{0.0F, 1.0F, 0.0F});
                cb->unmap();
            }

            renderDevice.setRasterState(nullptr);
            renderDevice.drawTrianglesIndexed(0, 6);

            {
                auto& mvp = *(glm::mat4*)cb->map();
                mvp = glm::perspective(glm::radians(70.0F), float(sz.x) / float(sz.y), 0.1F, 1000.0F) *
                      glm::lookAt(glm::vec3{0.0F, 0.0F, -5.0F}, glm::vec3{0.0F, 0.0F, 0.0F},
                                  glm::vec3{0.0F, 1.0F, 0.0F}) *
                      glm::translate(glm::mat4(1.0F), glm::vec3{2.0F, 0.0F, 0.0F}) *
                      glm::rotate(glm::mat4(1.0F), t, glm::vec3{0.0F, 1.0F, 0.0F});
                cb->unmap();
            }

            renderDevice.setRasterState(rs);
            renderDevice.drawTrianglesIndexed(0, 6);

            window->swapBuffers();
            while (window->pollEvent().has_value())
            {
                ; // Do nothing with events.
            }

            t += 0.01F;
        }
    }

    return 0;
}
