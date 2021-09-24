#include <cubos/gl/ogl_render_device.hpp>

#ifdef WITH_OPENGL

#include <glad/glad.h>

using namespace cubos::gl;

OGLRenderDevice::OGLRenderDevice()
{
    if (!gladLoadGL())
        ; // TODO: Log critical error and abort
}

void OGLRenderDevice::clearColor(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OGLRenderDevice::clearDepth(float depth) const
{
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OGLRenderDevice::clearStencil(int stencil) const
{
    glClearStencil(stencil);
    glClear(GL_STENCIL_BUFFER_BIT);
}

#endif // WITH_OPENGL
