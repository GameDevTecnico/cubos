#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;

int main()
{
    auto window = io::openWindow();
    auto& renderDevice = window->renderDevice();

    if (renderDevice.getProperty(gl::Property::ComputeSupported) == 0)
    {
        CUBOS_CRITICAL("Compute shaders are not supported on this device.");
        return 1;
    }

    {
        auto vs = renderDevice.createShaderStage(gl::Stage::Vertex, R"glsl(
            #version 330 core

            in vec2 position;
            in vec2 uv;

            out vec2 fragUV;

            void main()
            {
                fragUV = uv;
                gl_Position = vec4(position, 0.0f, 1.0f);
            }
        )glsl");

        auto ps = renderDevice.createShaderStage(gl::Stage::Pixel, R"glsl(
            #version 330 core

            in vec2 fragUV;
            out vec4 color;

            uniform sampler2D tex;

            void main()
            {
                color = vec4(texture(tex, fragUV).rgb, 1.0f);
            }
        )glsl");

        auto cs = renderDevice.createShaderStage(gl::Stage::Compute, R"glsl(
            #version 430 core

            layout(local_size_x = 16, local_size_y = 16) in;
            layout(binding = 0, rgba32f) uniform image2D tex;

            void main()
            {
                ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
                ivec2 size = imageSize(tex);
                vec2 uv = vec2(coords) / vec2(size);
                vec4 color = vec4(uv, 0.0f, 1.0f);
                imageStore(tex, coords, color);
            }
        )glsl");

        auto drawPp = renderDevice.createShaderPipeline(vs, ps);
        auto* drawTextureBP = drawPp->getBindingPoint("tex");
        auto computePp = renderDevice.createShaderPipeline(cs);
        auto* computeTextureBP = computePp->getBindingPoint("tex");

        float verts[] = {
            -1.0F, -1.0F, 0.0F, 0.0F, -1.0F, +1.0F, 0.0F, 1.0F, +1.0F, +1.0F, 1.0F, 1.0F, +1.0F, -1.0F, 1.0F, 0.0F,
        };

        auto vb = renderDevice.createVertexBuffer(sizeof(verts), verts, gl::Usage::Static);

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
        };

        auto ib = renderDevice.createIndexBuffer(sizeof(indices), indices, gl::IndexFormat::UInt, gl::Usage::Static);

        auto size = window->framebufferSize();
        gl::Texture2DDesc textureDesc;
        textureDesc.format = gl::TextureFormat::RGBA32Float;
        textureDesc.width = static_cast<std::size_t>(size.x);
        textureDesc.height = static_cast<std::size_t>(size.y);
        auto texture = renderDevice.createTexture2D(textureDesc);

        gl::SamplerDesc samplerDesc;
        samplerDesc.minFilter = gl::TextureFilter::Nearest;
        samplerDesc.magFilter = gl::TextureFilter::Nearest;
        samplerDesc.addressU = gl::AddressMode::Clamp;
        samplerDesc.addressV = gl::AddressMode::Clamp;
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
        vaDesc.shaderPipeline = drawPp;
        auto va = renderDevice.createVertexArray(vaDesc);

        while (!window->shouldClose())
        {
            renderDevice.setShaderPipeline(computePp);
            computeTextureBP->bind(texture, 0, gl::Access::Write);
            renderDevice.dispatchCompute((static_cast<std::size_t>(size.x) + 15) / 16,
                                         (static_cast<std::size_t>(size.y) + 15) / 16, 1);

            renderDevice.memoryBarrier(gl::MemoryBarriers::TextureAccess);

            renderDevice.setViewport(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
            renderDevice.setShaderPipeline(drawPp);
            renderDevice.setVertexArray(va);
            renderDevice.setIndexBuffer(ib);
            drawTextureBP->bind(texture);
            drawTextureBP->bind(sampler);
            renderDevice.drawTrianglesIndexed(0, 6);

            window->swapBuffers();
            while (window->pollEvent().has_value())
            {
                ; // Do nothing with events.
            }
        }
    }

    return 0;
}
