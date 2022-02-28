#ifndef CUBOS_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP
#define CUBOS_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP

#include <cubos/gl/render_device.hpp>
#include <cubos/io/window.hpp>
#include <cubos/rendering/renderer.hpp>

namespace cubos::rendering
{
    class Renderer;
    
    class PostProcessingPass
    {
    protected:
        io::Window& window;
        gl::RenderDevice& renderDevice;

    public:
        explicit PostProcessingPass(io::Window& window);

        virtual void execute(const Renderer& renderer, gl::Texture2D input, gl::Framebuffer output) const = 0;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_POST_PROCESSING_POST_PROCESSING_HPP
