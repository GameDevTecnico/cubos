#include <cubos/engine/rendering/post_processing/copy_pass.hpp>

using namespace cubos;
using namespace cubos::core;
using namespace cubos::core::gl;
using namespace cubos::engine;

engine::rendering::CopyPass::CopyPass(io::Window& window) : PostProcessingPass(window)
{
    auto vertex = renderDevice.createShaderStage(gl::Stage::Vertex, R"(
            #version 330 core

            in vec4 position;
            in vec2 uv;

            out vec2 fraguv;

            void main(void)
            {
                gl_Position = position;
                fraguv = uv;
            }
        )");

    auto pixel = renderDevice.createShaderStage(gl::Stage::Pixel, R"(
            #version 430 core

            in vec2 fraguv;

            uniform sampler2D inputTex;

            out vec4 color;

            void main()
            {
                color = texture(inputTex, fraguv);
            }
        )");

    pipeline = renderDevice.createShaderPipeline(vertex, pixel);

    renderDevice.setShaderPipeline(pipeline);

    inputTexBP = pipeline->getBindingPoint("inputTex");

    SamplerDesc samplerDesc;
    samplerDesc.addressU = gl::AddressMode::Clamp;
    samplerDesc.addressV = gl::AddressMode::Clamp;
    samplerDesc.magFilter = gl::TextureFilter::Nearest;
    samplerDesc.minFilter = gl::TextureFilter::Nearest;
    inputTexSampler = renderDevice.createSampler(samplerDesc);

    inputTexBP->bind(inputTexSampler);
}

void rendering::CopyPass::execute(const Renderer& renderer, Texture2D input, Framebuffer output) const
{
    renderDevice.setShaderPipeline(pipeline);
    renderDevice.setFramebuffer(output);
    inputTexBP->bind(input);

    auto sz = window.getFramebufferSize();

    renderDevice.setViewport(0, 0, sz.x, sz.y);
    renderDevice.clearColor(0, 0, 0, 0);

    renderDevice.setRasterState(nullptr);
    renderDevice.setBlendState(nullptr);
    renderDevice.setDepthStencilState(nullptr);

    VertexArray va;
    IndexBuffer ib;

    renderer.getScreenQuad(va, ib);

    renderDevice.setVertexArray(va);
    renderDevice.setIndexBuffer(ib);
    renderDevice.drawTrianglesIndexed(0, 6);
}