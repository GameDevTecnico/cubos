#pragma once

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    class ScreenPicker final
    {
    public:
        void init(cubos::core::gl::RenderDevice* currentRenderDevice, glm::ivec2 size);
        void resizeTexture(glm::ivec2 size);
        void clearTexture();
        core::gl::Framebuffer framebuffer();
        uint32_t at(int x, int y) const;
        glm::uvec2 size() const;

    private:
        cubos::core::gl::RenderDevice* renderDevice;
        core::gl::Framebuffer idFramebuffer;
        core::gl::Texture2D idTexture;
        glm::ivec2 textureSize;

        void initIdTexture(glm::ivec2 size);
    };
} // namespace cubos::engine
