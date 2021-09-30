#include <cubos/io/window.hpp>

#ifdef WITH_GLFW

#include <GLFW/glfw3.h>

#include <cubos/gl/ogl_render_device.hpp>

using namespace cubos;
using namespace cubos::io;

Window::Window()
{
    if (!glfwInit())
        ; // TODO: Log critical error and abort

    // TODO: Read settings and set the correct window size, title and mode (fullscreen, ...)
    this->handle = glfwCreateWindow(800, 600, "Cubos", nullptr, nullptr);
    if (!this->handle)
        ; // TODO: Log critical error and abort

    // Create OpenGL render device
    glfwMakeContextCurrent((GLFWwindow *)this->handle);
    this->renderDevice = new gl::OGLRenderDevice();
}

Window::~Window()
{
    delete this->renderDevice;
    glfwTerminate();
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::swapBuffers() const
{
    glfwSwapBuffers((GLFWwindow *)this->handle);
}

gl::RenderDevice &Window::getRenderDevice() const
{
    return *this->renderDevice;
}

glm::ivec2 Window::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow *)this->handle, &width, &height);
    return {width, height};
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose((GLFWwindow *)this->handle);
}

#endif // WITH_GLFW
