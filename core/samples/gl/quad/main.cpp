#include <cstdio>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core;

struct State
{
    io::Window window;
    gl::ShaderPipeline shaderPipeline;
    gl::VertexArray vertexArray;
    gl::IndexBuffer indexBuffer;
    gl::ShaderBindingPoint textureBP, constantBufferBP;
    gl::ConstantBuffer constantBuffer;
    gl::Sampler sampler;
    gl::Texture2D texture;
    gl::RasterState rasterState;

    float t;
};

void update(void* data)
{
    auto* state = static_cast<State*>(data);

    auto sz = state->window->framebufferSize();
    auto& renderDevice = state->window->renderDevice();

    renderDevice.setViewport(0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y));
    renderDevice.clearColor(0.894F, 0.592F, 0.141F, 0.0F);

    renderDevice.setShaderPipeline(state->shaderPipeline);
    renderDevice.setVertexArray(state->vertexArray);
    renderDevice.setIndexBuffer(state->indexBuffer);

    state->textureBP->bind(state->texture);
    state->textureBP->bind(state->sampler);
    state->constantBufferBP->bind(state->constantBuffer);

    auto mvp = glm::perspective(glm::radians(70.0F), float(sz.x) / float(sz.y), 0.1F, 1000.0F) *
               glm::lookAt(glm::vec3{0.0F, 0.0F, -5.0F}, glm::vec3{0.0F, 0.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F}) *
               glm::translate(glm::mat4(1.0F), glm::vec3{-2.0F, 0.0F, 0.0F}) *
               glm::rotate(glm::mat4(1.0F), state->t, glm::vec3{0.0F, 1.0F, 0.0F});
    state->constantBuffer->fill(&mvp, sizeof(mvp));

    renderDevice.setRasterState(nullptr);
    renderDevice.drawTrianglesIndexed(0, 6);

    mvp = glm::perspective(glm::radians(70.0F), float(sz.x) / float(sz.y), 0.1F, 1000.0F) *
          glm::lookAt(glm::vec3{0.0F, 0.0F, -5.0F}, glm::vec3{0.0F, 0.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F}) *
          glm::translate(glm::mat4(1.0F), glm::vec3{2.0F, 0.0F, 0.0F}) *
          glm::rotate(glm::mat4(1.0F), state->t, glm::vec3{0.0F, 1.0F, 0.0F});
    state->constantBuffer->fill(&mvp, sizeof(mvp));

    renderDevice.setRasterState(state->rasterState);
    renderDevice.drawTrianglesIndexed(0, 6);

    state->window->swapBuffers();
    while (state->window->pollEvent().has_value())
    {
        ; // Do nothing with events.
    }

    state->t += 0.01F;
}

int main()
{
    State state{};
    state.window = io::openWindow();
    auto& renderDevice = state.window->renderDevice();

    auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
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
            in vec2 fragUV;
            out vec4 color;

            uniform sampler2D colorTexture;

            void main()
            {
                color = vec4(texture(colorTexture, fragUV).rgb, 1.0f);
            }
        )");

    state.shaderPipeline = renderDevice.createShaderPipeline(vs, ps);

    float verts[] = {
        -0.5F, -0.5F, 0.0F, 0.0F, -0.5F, +0.5F, 0.0F, 1.0F, +0.5F, +0.5F, 1.0F, 1.0F, +0.5F, -0.5F, 1.0F, 0.0F,
    };

    auto vb = renderDevice.createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
    };

    state.indexBuffer =
        renderDevice.createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

    state.constantBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);
    state.constantBufferBP = state.shaderPipeline->getBindingPoint("MVP");

    float textureData[] = {
        0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F,
    };

    gl::Texture2DDesc textureDesc;
    textureDesc.data[0] = textureData;
    textureDesc.format = gl::TextureFormat::RGB32Float;
    textureDesc.width = 2;
    textureDesc.height = 2;
    state.texture = renderDevice.createTexture2D(textureDesc);
    state.textureBP = state.shaderPipeline->getBindingPoint("colorTexture");

    gl::SamplerDesc samplerDesc;
    samplerDesc.minFilter = gl::TextureFilter::Linear;
    samplerDesc.magFilter = gl::TextureFilter::Linear;
    samplerDesc.addressU = gl::AddressMode::Mirror;
    samplerDesc.addressV = gl::AddressMode::Mirror;
    state.sampler = renderDevice.createSampler(samplerDesc);

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
    vaDesc.shaderPipeline = state.shaderPipeline;
    state.vertexArray = renderDevice.createVertexArray(vaDesc);

    gl::RasterStateDesc rsDesc;
    state.rasterState = renderDevice.createRasterState(rsDesc);

    state.t = 0.0F;

#ifndef __EMSCRIPTEN__
    while (!state.window->shouldClose())
    {
        update(&state);
    }
#else
    emscripten_set_main_loop_arg(update, &state, 0, 1);
#endif

    return 0;
}