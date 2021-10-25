#include <cubos/io/window.hpp>

#include <cubos/gl/ogl_render_device.hpp>

using namespace cubos;
using namespace cubos::io;

void Window::initRenderDevice()
{
    this->renderDevice = new gl::OGLRenderDevice();
}

void Window::destroyRenderDevice()
{
    delete this->renderDevice;
}

gl::RenderDevice &Window::getRenderDevice() const
{
    return *this->renderDevice;
}
