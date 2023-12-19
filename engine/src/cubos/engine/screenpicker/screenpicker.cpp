#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/screenpicker/screenpicker.hpp>

using cubos::engine::ScreenPicker;

using namespace cubos::core::gl;

void ScreenPicker::init(cubos::core::gl::RenderDevice* currentRenderDevice, glm::ivec2 size)
{
    renderDevice = currentRenderDevice;
    initIdTexture(size);
}

void ScreenPicker::resizeTexture(glm::ivec2 size)
{
    initIdTexture(size);
}

void ScreenPicker::clearTexture()
{
    auto* texBuffer = new uint16_t[(std::size_t)textureSize.x * (std::size_t)textureSize.y * 2U];

    // UINT32_MAX mean 'nothing's here'
    // Since the texture is in RG16UInt format, it's filled with UINT16_MAX instead (two values make up an identifier)
    std::fill(texBuffer, texBuffer + (std::size_t)textureSize.x * (std::size_t)textureSize.y * 2U, UINT16_MAX);

    idTexture->update(0, 0, textureSize.x, textureSize.y, texBuffer);

    delete[] texBuffer;
}

Framebuffer ScreenPicker::framebuffer()
{
    return idFramebuffer;
}

uint32_t ScreenPicker::at(int x, int y) const
{
    auto* texBuffer = new uint16_t[(std::size_t)textureSize.x * (std::size_t)textureSize.y * 2U];

    idTexture->read(texBuffer);

    uint16_t r = texBuffer[(ptrdiff_t)(y * textureSize.x + x) * 2U];
    uint16_t g = texBuffer[(ptrdiff_t)(y * textureSize.x + x) * 2U + 1U];

    uint32_t id = (static_cast<uint32_t>(r) << 16U) | g;

    delete[] texBuffer;

    return id;
}

glm::uvec2 ScreenPicker::size() const
{
    return static_cast<glm::uvec2>(textureSize);
}

void ScreenPicker::initIdTexture(glm::ivec2 size)
{
    Texture2DDesc texDesc;
    texDesc.width = (std::size_t)size.x;
    texDesc.height = (std::size_t)size.y;
    texDesc.usage = Usage::Dynamic;
    texDesc.format = TextureFormat::RG16UInt;

    idTexture = renderDevice->createTexture2D(texDesc);

    FramebufferDesc frameDesc;
    frameDesc.targetCount = 1;
    frameDesc.targets[0].setTexture2DTarget(idTexture);

    idFramebuffer = renderDevice->createFramebuffer(frameDesc);

    textureSize = size;
}
