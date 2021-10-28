#include <cubos/io/glfw_window.hpp>

#include <cubos/gl/ogl_render_device.hpp>

#include <cubos/log.hpp>

#ifndef GLAD_LOADED
#define GLAD_LOADED
#include <glad/glad.h>
#endif // GLAD_LOADED

using namespace cubos;
using namespace cubos::io;

void static keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS)
    {
        handler->keyboardKeyDown.fire(key);
    }
    else if (action == GLFW_RELEASE)
    {
        handler->keyboardKeyUp.fire(key);
    }
}

void static mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->mousePosition.fire(glm::ivec2(xPos, yPos));
}

void static mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        handler->mouseRightClickDown.fire();
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        handler->mouseRightClickUp.fire();
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        handler->mouseLeftClickDown.fire();
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        handler->mouseLeftClickUp.fire();
    }
}

GLFWWindow::GLFWWindow()
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
    glfwMakeContextCurrent(this->handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logCritical("GLFWWindowImpl::GLFWWindowImpl() failed: OpenGL loader failed");
        abort();
    }
    this->renderDevice = new gl::OGLRenderDevice();
    glfwSetWindowUserPointer(this->handle, (void*)this);
    glfwSetKeyCallback(this->handle, keyCallback);
    glfwSetCursorPosCallback(this->handle, mousePositionCallback);
    glfwSetMouseButtonCallback(this->handle, mouseButtonCallback);
}

GLFWWindow::~GLFWWindow()
{
    delete this->renderDevice;
    glfwTerminate();
}

void GLFWWindow::pollEvents() const
{
    glfwPollEvents();
}

void GLFWWindow::swapBuffers() const
{
    glfwSwapBuffers(this->handle);
}

gl::RenderDevice& GLFWWindow::getRenderDevice() const
{
    return *this->renderDevice;
}

glm::ivec2 GLFWWindow::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(this->handle, &width, &height);
    return {width, height};
}

bool GLFWWindow::shouldClose() const
{
    return glfwWindowShouldClose(this->handle);
}
