#include <cubos/engine/gl/pps/manager.hpp>
#include <cubos/engine/gl/pps/pass.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

pps::Manager::Manager(RenderDevice& renderDevice, glm::uvec2 size)
    : renderDevice(renderDevice)
{
    this->nextId = 0;
    this->size = {0, 0};
    this->resize(size);
}

pps::Manager::~Manager()
{
    for (auto& pass : this->passes)
    {
        delete pass.second;
    }
}

void pps::Manager::resize(glm::uvec2 size)
{
    // Only resize if the size changed.
    if (this->size == size)
    {
        return;
    }
    this->size = size;
    for (auto& pass : this->passes)
    {
        pass.second->resize(size);
    }

    // Create the intermediate texture.
    Texture2DDesc desc;
    desc.width = this->size.x;
    desc.height = this->size.y;
    desc.format = TextureFormat::RGBA32Float;
    desc.usage = Usage::Dynamic;
    this->intermediateTex[0] = this->renderDevice.createTexture2D(desc);
    this->intermediateTex[1] = this->renderDevice.createTexture2D(desc);

    // Create the intermediate framebuffer.
    FramebufferDesc fbDesc;
    fbDesc.targetCount = 1;
    fbDesc.targets[0].setTexture2DTarget(this->intermediateTex[0]);
    this->intermediateFb[0] = this->renderDevice.createFramebuffer(fbDesc);
    fbDesc.targets[0].setTexture2DTarget(this->intermediateTex[1]);
    this->intermediateFb[1] = this->renderDevice.createFramebuffer(fbDesc);
}

void pps::Manager::provideInput(Input input, Texture2D texture)
{
    this->inputs[input] = texture;
}

void pps::Manager::removePass(size_t id)
{
    delete this->passes[id];
    this->passes.erase(id);
}

void pps::Manager::execute(Framebuffer out)
{
    auto prev = this->inputs.at(Input::Lighting);
    size_t nextI = 0;

    for (auto it = this->passes.begin(); it != this->passes.end(); ++it)
    {
        auto nextIt = it;
        ++nextIt;

        if (nextIt == this->passes.end())
        {
            // If the pass is the last one, render to the output framebuffer.
            it->second->execute(this->inputs, prev, out);
        }
        else
        {
            // Otherwise, render to the intermediate framebuffer.
            it->second->execute(this->inputs, prev, this->intermediateFb[nextI]);
            prev = this->intermediateTex[nextI];
            nextI = (nextI + 1) % 2;
        }
    }
}

size_t pps::Manager::passCount() const
{
    return this->passes.size();
}
