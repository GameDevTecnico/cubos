#include <cubos/io/glfw_window.hpp>

#include <cubos/gl/ogl_render_device.hpp>

#include <cubos/log.hpp>

#include <glad/glad.h>

using namespace cubos;
using namespace cubos::io;

GLFWWindow::GLFWWindow()
{
#ifdef WITH_GLFW
    if (!glfwInit())
    {
        logCritical("OGLRenderDevice::OGLRenderDevice() failed: glfwInit() failed");
        abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // TODO: Read settings and set the correct window size, title and mode (fullscreen, ...)
    this->handle = glfwCreateWindow(800, 600, "Cubos", nullptr, nullptr);
    if (!this->handle)
        ; // TODO: Log critical error and abort

    // Create OpenGL render device
    glfwMakeContextCurrent(this->handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logCritical("GLFWWindow::GLFWWindow() failed: OpenGL loader failed");
        abort();
    }
    this->renderDevice = new gl::OGLRenderDevice();
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif // WITH_GLFW
}

GLFWWindow::~GLFWWindow()
{
#ifdef WITH_GLFW
    delete this->renderDevice;
    glfwTerminate();
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}

void GLFWWindow::pollEvents() const
{
#ifdef WITH_GLFW
    glfwPollEvents();
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}

void GLFWWindow::swapBuffers() const
{
#ifdef WITH_GLFW
    glfwSwapBuffers(this->handle);
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}

gl::RenderDevice& GLFWWindow::getRenderDevice() const
{
#ifdef WITH_GLFW
    return *this->renderDevice;
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}

glm::ivec2 GLFWWindow::getFramebufferSize() const
{
#ifdef WITH_GLFW
    int width, height;
    glfwGetFramebufferSize(this->handle, &width, &height);
    return {width, height};
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}

bool GLFWWindow::shouldClose() const
{
#ifdef WITH_GLFW
    return glfwWindowShouldClose(this->handle);
#else
    logCritical("GLFWWindow::GLFWWindow() failed: Building without GLFW, not supported");
    abort();
#endif
}
