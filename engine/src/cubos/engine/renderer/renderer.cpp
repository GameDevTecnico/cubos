#include <cubos/engine/renderer/renderer.hpp>

using cubos::core::gl::RenderDevice;
using cubos::engine::BaseRenderer;

BaseRenderer::BaseRenderer(RenderDevice& renderDevice, glm::uvec2 size)
    : mRenderDevice(renderDevice)
    , mPpsManager(renderDevice, size)
    , mSize(size)
{
    this->resizeTex(size);
}

void BaseRenderer::resize(glm::uvec2 size)
{
    mSize = size;
    this->resizeTex(size);
    mPpsManager.resize(size);
    this->onResize(size);
}

glm::uvec2 BaseRenderer::size() const
{
    return mSize;
}

void BaseRenderer::render(const glm::mat4& view, const Viewport& viewport, const engine::Camera& camera,
                          const RendererFrame& frame, const core::gl::Framebuffer& pickingBuffer,
                          bool enableScreenPicking, bool usePostProcessing, const core::gl::Framebuffer& target)
{
    if (usePostProcessing && mPpsManager.passCount() > 0)
    {
        this->onRender(view, viewport, camera, frame, mFramebuffer, pickingBuffer, enableScreenPicking);
        mPpsManager.provideInput(PostProcessingInput::Lighting, mTexture);
        mPpsManager.execute(target);
    }
    else
    {
        this->onRender(view, viewport, camera, frame, target, pickingBuffer, enableScreenPicking);
    }
}

cubos::engine::PostProcessingManager& BaseRenderer::pps()
{
    return mPpsManager;
}

void BaseRenderer::resizeTex(glm::uvec2 size)
{
    core::gl::Texture2DDesc textureDesc;
    textureDesc.format = core::gl::TextureFormat::RGBA32Float;
    textureDesc.width = size.x;
    textureDesc.height = size.y;
    mTexture = mRenderDevice.createTexture2D(textureDesc);

    core::gl::FramebufferDesc framebufferDesc;
    framebufferDesc.targetCount = 1;
    framebufferDesc.targets[0].setTexture2DTarget(mTexture);
    mFramebuffer = mRenderDevice.createFramebuffer(framebufferDesc);
}
