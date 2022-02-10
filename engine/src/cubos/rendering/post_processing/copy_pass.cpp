#include "cubos/rendering/post_processing/copy_pass.hpp"

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

cubos::rendering::CopyPass::CopyPass(cubos::io::Window& window) : PostProcessingPass(window)
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
                color = vec4(texture(inputTex, fraguv).rgb,1);
                //color += vec4(fraguv,0,1);
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

    float screenVerts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, +1.0f, 0.0f, 1.0f, +1.0f, +1.0f, 1.0f, 1.0f, +1.0f, -1.0f, 1.0f, 0.0f,
    };
    auto screenVertexBuffer = renderDevice.createVertexBuffer(sizeof(screenVerts), screenVerts, gl::Usage::Static);

    VertexArrayDesc screenVADesc;
    screenVADesc.elementCount = 2;
    screenVADesc.elements[0].name = "position";
    screenVADesc.elements[0].type = gl::Type::Float;
    screenVADesc.elements[0].size = 2;
    screenVADesc.elements[0].buffer.index = 0;
    screenVADesc.elements[0].buffer.offset = 0;
    screenVADesc.elements[0].buffer.stride = 4 * sizeof(float);
    screenVADesc.elements[1].name = "uv";
    screenVADesc.elements[1].type = gl::Type::Float;
    screenVADesc.elements[1].size = 2;
    screenVADesc.elements[1].buffer.index = 0;
    screenVADesc.elements[1].buffer.offset = 2 * sizeof(float);
    screenVADesc.elements[1].buffer.stride = 4 * sizeof(float);
    screenVADesc.buffers[0] = screenVertexBuffer;
    screenVADesc.shaderPipeline = pipeline;
    screenVertexArray = renderDevice.createVertexArray(screenVADesc);

    unsigned int screenIndices[] = {
        0, 1, 2, 2, 3, 0,
    };
    screenIndexBuffer =
        renderDevice.createIndexBuffer(sizeof(screenIndices), screenIndices, gl::IndexFormat::UInt, gl::Usage::Static);
}

void cubos::rendering::CopyPass::execute(const Renderer& renderer, gl::Texture2D input, gl::Framebuffer output) const
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

    renderDevice.setVertexArray(screenVertexArray);
    renderDevice.setIndexBuffer(screenIndexBuffer);
    renderDevice.drawTrianglesIndexed(0, 6);
}