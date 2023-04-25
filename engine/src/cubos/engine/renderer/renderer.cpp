#include <cubos/engine/renderer/renderer.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

BaseRenderer::BaseRenderer(RenderDevice& renderDevice, glm::uvec2 size)
    : mRenderDevice(renderDevice)
    , mPpsManager(renderDevice, size)
{
    this->resizeTex(size);
}

void BaseRenderer::resize(glm::uvec2 size)
{
    this->resizeTex(size);
    mPpsManager.resize(size);
    this->onResize(size);
}

void BaseRenderer::render(const Camera& camera, const Frame& frame, bool usePostProcessing,
                          const core::gl::Framebuffer& target)
{
    if (usePostProcessing && mPpsManager.passCount() > 0)
    {
        this->onRender(camera, frame, mFramebuffer);
        mPpsManager.provideInput(pps::Input::Lighting, mTexture);
        mPpsManager.execute(target);
    }
    else
    {
        this->onRender(camera, frame, target);
    }
}

pps::Manager& BaseRenderer::pps()
{
    return mPpsManager;
}

void BaseRenderer::resizeTex(glm::uvec2 size)
{
    Texture2DDesc textureDesc;
    textureDesc.format = TextureFormat::RGBA32Float;
    textureDesc.width = size.x;
    textureDesc.height = size.y;
    mTexture = mRenderDevice.createTexture2D(textureDesc);

    FramebufferDesc framebufferDesc;
    framebufferDesc.targetCount = 1;
    framebufferDesc.targets[0].setTexture2DTarget(mTexture);
    mFramebuffer = mRenderDevice.createFramebuffer(framebufferDesc);
}
