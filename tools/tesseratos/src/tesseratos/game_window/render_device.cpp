#include "render_device.hpp"
#include <algorithm>

using namespace cubos::core::gl;
using namespace tesseratos;

GameRenderDevice::GameRenderDevice(RenderDevice& renderDevice)
    : mRenderDevice(renderDevice)
{
}

void GameRenderDevice::createSwapChain(const glm::uvec2& size)
{
    Texture2DDesc texDesc{
        .width = size.x,
        .height = size.y,
        .usage = Usage::Dynamic,
        .format = TextureFormat::RGBA8UNorm,
    };
    mFrontTexture = mRenderDevice.createTexture2D(texDesc);
    mBackTexture = mRenderDevice.createTexture2D(texDesc);

    Texture2DDesc depthStencilDesc{
        .width = size.x,
        .height = size.y,
        .usage = Usage::Dynamic,
        .format = TextureFormat::Depth24Stencil8,
    };

    FramebufferDesc fbDesc{};
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(mFrontTexture);
    fbDesc.depthStencil.setTexture2DTarget(mRenderDevice.createTexture2D(depthStencilDesc));
    mFrontFramebuffer = mRenderDevice.createFramebuffer(fbDesc);
    fbDesc.targets[0].setTexture2DTarget(mBackTexture);
    fbDesc.depthStencil.setTexture2DTarget(mRenderDevice.createTexture2D(depthStencilDesc));
    mBackFramebuffer = mRenderDevice.createFramebuffer(fbDesc);

    if (mCurrentFramebuffer == nullptr)
    {
        mRenderDevice.setFramebuffer(mBackFramebuffer);
    }
}

void GameRenderDevice::swapBuffers()
{
    std::swap(mFrontFramebuffer, mBackFramebuffer);
    std::swap(mFrontTexture, mBackTexture);

    if (mCurrentFramebuffer == nullptr)
    {
        mRenderDevice.setFramebuffer(mBackFramebuffer);
    }
}

Texture2D GameRenderDevice::output() const
{
    return mFrontTexture;
}

Framebuffer GameRenderDevice::createFramebuffer(const FramebufferDesc& desc)
{
    return mRenderDevice.createFramebuffer(desc);
}

void GameRenderDevice::setFramebuffer(Framebuffer fb)
{
    if (fb == nullptr)
    {
        mRenderDevice.setFramebuffer(mBackFramebuffer);
    }
    else
    {
        mRenderDevice.setFramebuffer(fb);
    }

    mCurrentFramebuffer = fb;
}

RasterState GameRenderDevice::createRasterState(const RasterStateDesc& desc)
{
    return mRenderDevice.createRasterState(desc);
}

void GameRenderDevice::setRasterState(RasterState rs)
{
    mRenderDevice.setRasterState(rs);
}

DepthStencilState GameRenderDevice::createDepthStencilState(const DepthStencilStateDesc& desc)
{
    return mRenderDevice.createDepthStencilState(desc);
}

void GameRenderDevice::setDepthStencilState(DepthStencilState dss)
{
    mRenderDevice.setDepthStencilState(dss);
}

BlendState GameRenderDevice::createBlendState(const BlendStateDesc& desc)
{
    return mRenderDevice.createBlendState(desc);
}

void GameRenderDevice::setBlendState(BlendState bs)
{
    mRenderDevice.setBlendState(bs);
}

Sampler GameRenderDevice::createSampler(const SamplerDesc& desc)
{
    return mRenderDevice.createSampler(desc);
}

Texture1D GameRenderDevice::createTexture1D(const Texture1DDesc& desc)
{
    return mRenderDevice.createTexture1D(desc);
}

Texture2D GameRenderDevice::createTexture2D(const Texture2DDesc& desc)
{
    return mRenderDevice.createTexture2D(desc);
}

Texture2DArray GameRenderDevice::createTexture2DArray(const Texture2DArrayDesc& desc)
{
    return mRenderDevice.createTexture2DArray(desc);
}

Texture3D GameRenderDevice::createTexture3D(const Texture3DDesc& desc)
{
    return mRenderDevice.createTexture3D(desc);
}

CubeMap GameRenderDevice::createCubeMap(const CubeMapDesc& desc)
{
    return mRenderDevice.createCubeMap(desc);
}

CubeMapArray GameRenderDevice::createCubeMapArray(const CubeMapArrayDesc& desc)
{
    return mRenderDevice.createCubeMapArray(desc);
}

PixelPackBuffer GameRenderDevice::createPixelPackBuffer(std::size_t size)
{
    return mRenderDevice.createPixelPackBuffer(size);
}

ConstantBuffer GameRenderDevice::createConstantBuffer(std::size_t size, const void* data, Usage usage)
{
    return mRenderDevice.createConstantBuffer(size, data, usage);
}

IndexBuffer GameRenderDevice::createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage)
{
    return mRenderDevice.createIndexBuffer(size, data, format, usage);
}

void GameRenderDevice::setIndexBuffer(IndexBuffer ib)
{
    mRenderDevice.setIndexBuffer(ib);
}

VertexBuffer GameRenderDevice::createVertexBuffer(std::size_t size, const void* data, Usage usage)
{
    return mRenderDevice.createVertexBuffer(size, data, usage);
}

VertexArray GameRenderDevice::createVertexArray(const VertexArrayDesc& desc)
{
    return mRenderDevice.createVertexArray(desc);
}

void GameRenderDevice::setVertexArray(VertexArray va)
{
    mRenderDevice.setVertexArray(va);
}

ShaderStage GameRenderDevice::createShaderStage(Stage stage, const char* src)
{
    return mRenderDevice.createShaderStage(stage, src);
}

ShaderPipeline GameRenderDevice::createShaderPipeline(ShaderStage vs, ShaderStage fs)
{
    return mRenderDevice.createShaderPipeline(vs, fs);
}

ShaderPipeline GameRenderDevice::createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage fs)
{
    return mRenderDevice.createShaderPipeline(vs, gs, fs);
}

ShaderPipeline GameRenderDevice::createShaderPipeline(ShaderStage cs)
{
    return mRenderDevice.createShaderPipeline(cs);
}

void GameRenderDevice::setShaderPipeline(ShaderPipeline pipeline)
{
    mRenderDevice.setShaderPipeline(pipeline);
}

void GameRenderDevice::clearColor(float r, float g, float b, float a)
{
    mRenderDevice.clearColor(r, g, b, a);
}

void GameRenderDevice::clearTargetColor(std::size_t target, float r, float g, float b, float a)
{
    mRenderDevice.clearTargetColor(target, r, g, b, a);
}

void GameRenderDevice::clearTargetColor(std::size_t target, int r, int g, int b, int a)
{
    mRenderDevice.clearTargetColor(target, r, g, b, a);
}

void GameRenderDevice::clearDepth(float depth)
{
    mRenderDevice.clearDepth(depth);
}

void GameRenderDevice::clearStencil(int stencil)
{
    mRenderDevice.clearStencil(stencil);
}

void GameRenderDevice::drawLines(std::size_t offset, std::size_t count)
{
    mRenderDevice.drawLines(offset, count);
}

void GameRenderDevice::drawTriangles(std::size_t offset, std::size_t count)
{
    mRenderDevice.drawTriangles(offset, count);
}

void GameRenderDevice::drawTrianglesIndexed(std::size_t offset, std::size_t count)
{
    mRenderDevice.drawTrianglesIndexed(offset, count);
}

void GameRenderDevice::drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount)
{
    mRenderDevice.drawTrianglesInstanced(offset, count, instanceCount);
}

void GameRenderDevice::drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount)
{
    mRenderDevice.drawTrianglesIndexedInstanced(offset, count, instanceCount);
}

void GameRenderDevice::dispatchCompute(std::size_t x, std::size_t y, std::size_t z)
{
    mRenderDevice.dispatchCompute(x, y, z);
}

void GameRenderDevice::memoryBarrier(MemoryBarriers barriers)
{
    mRenderDevice.memoryBarrier(barriers);
}

void GameRenderDevice::setViewport(int x, int y, int w, int h)
{
    mRenderDevice.setViewport(x, y, w, h);
}

void GameRenderDevice::setScissor(int x, int y, int w, int h)
{
    mRenderDevice.setScissor(x, y, w, h);
}

int GameRenderDevice::getProperty(Property prop)
{
    return mRenderDevice.getProperty(prop);
}
