#include <cubos/rendering/post_processing/hdr_tonemapping_pass.hpp>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

HDRTonemappingPass::HDRTonemappingPass(cubos::io::Window& window) : PostProcessingPass(window)
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

            uniform float exposure;
            uniform float gamma;

            uniform sampler2D inputTex;

            out vec4 color;

            void main()
            {
                vec4 inputColor = texture(inputTex, fraguv);

                vec3 mapped = vec3(1.0) - exp(-inputColor.rgb * exposure);

                mapped = pow(mapped, vec3(1.0 / gamma));

                color = vec4(mapped, inputColor.a);
            }
        )");

    pipeline = renderDevice.createShaderPipeline(vertex, pixel);

    renderDevice.setShaderPipeline(pipeline);

    exposureBP = pipeline->getBindingPoint("exposure");
    gammaBP = pipeline->getBindingPoint("gamma");
    inputTexBP = pipeline->getBindingPoint("inputTex");

    SamplerDesc samplerDesc;
    samplerDesc.addressU = gl::AddressMode::Clamp;
    samplerDesc.addressV = gl::AddressMode::Clamp;
    samplerDesc.magFilter = gl::TextureFilter::Nearest;
    samplerDesc.minFilter = gl::TextureFilter::Nearest;
    inputTexSampler = renderDevice.createSampler(samplerDesc);

    inputTexBP->bind(inputTexSampler);
}

HDRTonemappingPass::HDRTonemappingPass(cubos::io::Window& window, float exposure, float gamma)
    : HDRTonemappingPass(window)
{
    this->exposure = exposure;
    this->gamma = gamma;
}

void HDRTonemappingPass::execute(const Renderer& renderer, gl::Texture2D input, gl::Framebuffer output) const
{
    renderDevice.setShaderPipeline(pipeline);
    renderDevice.setFramebuffer(output);
    inputTexBP->bind(input);

    exposureBP->setConstant(exposure);
    gammaBP->setConstant(gamma);

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
