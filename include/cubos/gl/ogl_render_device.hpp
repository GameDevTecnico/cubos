#ifndef CUBOS_GL_OGL_RENDER_DEVICE_HPP
#define CUBOS_GL_OGL_RENDER_DEVICE_HPP

#include <cubos/gl/render_device.hpp>

namespace cubos::gl
{
    // TODO: Add documentation

    class OGLRenderDevice : public RenderDevice
    {
    public:
        OGLRenderDevice();

        virtual void clearColor(float r, float g, float b, float a) const override;
        virtual void clearDepth(float depth) const override;
        virtual void clearStencil(int stencil) const override;
    };
} // namespace cubos::gl

#endif // CUBOS_GL_OGL_RENDER_DEVICE_HPP
