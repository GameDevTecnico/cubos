#include <cubos/engine/gl/renderer.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

Renderer::Renderer(RenderDevice& renderDevice, glm::uvec2 size)
    : renderDevice(renderDevice), ppsManager(renderDevice, size)
{
    Texture2DDesc textureDesc;
    textureDesc.format = TextureFormat::RGBA32Float;
    textureDesc.width = size.x;
    textureDesc.height = size.y;
    this->texture = renderDevice.createTexture2D(textureDesc);

    FramebufferDesc framebufferDesc;
    framebufferDesc.targetCount = 1;
    framebufferDesc.targets[0].setTexture2DTarget(this->texture);
    this->framebuffer = renderDevice.createFramebuffer(framebufferDesc);
}

void Renderer::resize(glm::uvec2 size)
{
    this->ppsManager.resize(size);
    this->onResize(size);
}

void Renderer::render(const Camera& camera, const Frame& frame, bool usePostProcessing, core::gl::Framebuffer target)
{
    if (usePostProcessing)
    {
        this->onRender(camera, frame, this->framebuffer);
        this->ppsManager.provideInput(pps::Input::Lighting, this->texture);
        this->ppsManager.execute(target);
    }
    else
    {
        this->onRender(camera, frame, target);
    }
}
