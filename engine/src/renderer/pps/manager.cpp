#include <utility>

#include <cubos/engine/renderer/pps/manager.hpp>
#include <cubos/engine/renderer/pps/pass.hpp>

using namespace cubos::core::gl;
using cubos::engine::PostProcessingInput;
using cubos::engine::PostProcessingManager;

PostProcessingManager::PostProcessingManager(RenderDevice& renderDevice, glm::uvec2 size)
    : mRenderDevice(renderDevice)
{
    mNextId = 0;
    mSize = {0, 0};
    this->resize(size);
}

PostProcessingManager::~PostProcessingManager()
{
    for (auto& pass : mPasses)
    {
        delete pass.second;
    }
}

void PostProcessingManager::resize(glm::uvec2 size)
{
    // Only resize if the size changed.
    if (mSize == size)
    {
        return;
    }
    mSize = size;
    for (auto& pass : mPasses)
    {
        pass.second->resize(size);
    }

    // Create the intermediate texture.
    Texture2DDesc desc;
    desc.width = mSize.x;
    desc.height = mSize.y;
    desc.format = TextureFormat::RGBA32Float;
    desc.usage = Usage::Dynamic;
    mIntermediateTex[0] = mRenderDevice.createTexture2D(desc);
    mIntermediateTex[1] = mRenderDevice.createTexture2D(desc);

    // Create the intermediate framebuffer.
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(mIntermediateTex[0]);
    mIntermediateFb[0] = mRenderDevice.createFramebuffer(fbDesc);
    fbDesc.targets[0].setTexture2DTarget(mIntermediateTex[1]);
    mIntermediateFb[1] = mRenderDevice.createFramebuffer(fbDesc);
}

void PostProcessingManager::provideInput(PostProcessingInput input, Texture2D texture)
{
    mInputs[input] = std::move(texture);
}

void PostProcessingManager::removePass(std::size_t id)
{
    delete mPasses[id];
    mPasses.erase(id);
}

void PostProcessingManager::execute(const Framebuffer& out)
{
    auto prev = mInputs.at(PostProcessingInput::Lighting);
    std::size_t nextI = 0;

    for (auto it = mPasses.begin(); it != mPasses.end(); ++it)
    {
        auto nextIt = it;
        ++nextIt;

        if (nextIt == mPasses.end())
        {
            // If the pass is the last one, render to the output framebuffer.
            it->second->execute(mInputs, prev, out);
        }
        else
        {
            // Otherwise, render to the intermediate framebuffer.
            it->second->execute(mInputs, prev, mIntermediateFb[nextI]);
            prev = mIntermediateTex[nextI];
            nextI = (nextI + 1) % 2;
        }
    }
}

std::size_t PostProcessingManager::passCount() const
{
    return mPasses.size();
}
