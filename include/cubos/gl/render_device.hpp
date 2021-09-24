#ifndef CUBOS_GL_RENDER_DEVICE_HPP
#define CUBOS_GL_RENDER_DEVICE_HPP

namespace cubos::gl
{
    /// Abstract render device type, used to wrap low-level rendering APIs such as OpenGL
    /// @see io::Window, gl::OGLRenderDevice
    class RenderDevice
    {
    public:
        RenderDevice() = default;
        virtual ~RenderDevice() = default;
        RenderDevice(const RenderDevice &) = delete;

        /// Clears the color buffer bit on the current framebuffer to a specific color
        virtual void clearColor(float r, float g, float b, float a) const = 0;

        /// Clears the depth buffer bit on the current framebuffer to a specific value
        virtual void clearDepth(float depth) const = 0;

        /// Clears the stencil buffer bit on the current framebuffer to a specific value
        virtual void clearStencil(int stencil) const = 0;
    };
} // namespace cubos::gl

#endif // CUBOS_GL_RENDER_DEVICE_HPP
