#include <cubos/core/io/window.hpp>
#include <cubos/core/io/glfw_window.hpp>

using namespace cubos::core::io;

Window* Window::create()
{
    return new GLFWWindow();
}
