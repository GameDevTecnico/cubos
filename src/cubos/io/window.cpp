#include <cubos/io/window.hpp>
#include <cubos/io/glfw_window.hpp>

using namespace cubos::io;

Window* Window::create()
{
    return new GLFWWindow();
}
