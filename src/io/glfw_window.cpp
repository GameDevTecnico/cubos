#include <cubos/io/window.hpp>

#ifdef WITH_GLFW

#include <GLFW/glfw3.h>

using namespace cubos::io;

Window::Window()
{
    // TODO: Read settings and set the correct window size, title and mode (fullscreen, ...)
    this->handle = glfwCreateWindow(800, 600, "Cubos", nullptr, nullptr);
    if (!this->handle)
        ; // TODO: Log critical error and abort
}

Window::~Window()
{
    glfwDestroyWindow((GLFWwindow *)this->handle);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::swapBuffers() const
{
    glfwSwapBuffers((GLFWwindow*)this->handle);
}

glm::ivec2 Window::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)this->handle, &width, &height);
    return { width, height };
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose((GLFWwindow*)this->handle);
}

#endif // WITH_GLFW
