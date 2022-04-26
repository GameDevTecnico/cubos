#ifndef CUBOS_ENGINE_GL_PPS_PASS_HPP
#define CUBOS_ENGINE_GL_PPS_PASS_HPP

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/engine/gl/renderer.hpp>

namespace cubos::engine::gl
{
    class Renderer;
}

namespace cubos::engine::gl::pps
{
    /// A generic post procresssing pass.
    class Pass
    {
    protected:
        core::io::Window& window;
        core::gl::RenderDevice& renderDevice;

    public:
        explicit Pass(core::io::Window& window);

        virtual void execute(const gl::Renderer& renderer, core::gl::Texture2D input,
                             core::gl::Framebuffer output) const = 0;
    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_PASS_HPP
