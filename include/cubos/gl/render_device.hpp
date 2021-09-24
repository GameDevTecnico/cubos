#ifndef CUBOS_GL_RENDER_DEVICE_HPP
#define CUBOS_GL_RENDER_DEVICE_HPP

namespace cubos::gl
{
    class RenderDevice
    {
    public:
        RenderDevice() = default;
        virtual ~RenderDevice() = default;
        RenderDevice(const RenderDevice&) = delete;

        virtual void clearColor(float r, float g, float b, float a) const = 0;
        virtual void clearDepth(float depth) const = 0;
        virtual void clearStencil(int stencil) const = 0;
    };
} // namespace cubos::gl

#endif // CUBOS_GL_RENDER_DEVICE_HPP
