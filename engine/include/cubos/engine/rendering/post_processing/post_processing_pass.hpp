#ifndef CUBOS_ENGINE_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP
#define CUBOS_ENGINE_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/engine/rendering/renderer.hpp>

namespace cubos::engine::rendering
{
    class Renderer;

    class PostProcessingPass
    {
    protected:
        core::io::Window& window;
        core::gl::RenderDevice& renderDevice;

    public:
        explicit PostProcessingPass(core::io::Window& window);

        virtual void execute(const Renderer& renderer, core::gl::Texture2D input,
                             core::gl::Framebuffer output) const = 0;
    };
} // namespace cubos::engine::rendering

#endif // CUBOS_ENGINE_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP
