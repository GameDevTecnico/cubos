#include <cubos/io/glfw/glfw_window_impl.hpp>

#include <cubos/gl/ogl_render_device.hpp>

#include <cubos/log.hpp>

#ifndef GLAD_LOADED
#define GLAD_LOADED
#include <glad/glad.h>
#endif // GLAD_LOADED

using namespace cubos;
using namespace cubos::io;
using namespace cubos::io::glfw;

GLFWWindowImpl::GLFWWindowImpl()
{
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
    glfwMakeContextCurrent((GLFWwindow*)this->handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logCritical("GLFWWindowImpl::GLFWWindowImpl() failed: OpenGL loader failed");
        abort();
    }
    initRenderDevice();
}

GLFWWindowImpl::~GLFWWindowImpl()
{
    destroyRenderDevice();
    glfwTerminate();
}

void GLFWWindowImpl::pollEvents() const
{
    glfwPollEvents();
}

void GLFWWindowImpl::swapBuffers() const
{
    glfwSwapBuffers((GLFWwindow*)this->handle);
}

glm::ivec2 GLFWWindowImpl::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)this->handle, &width, &height);
    return {width, height};
}

bool GLFWWindowImpl::shouldClose() const
{
    return glfwWindowShouldClose((GLFWwindow*)this->handle);
}
