#include <cubos/core/io/window.hpp>
#include <cubos/core/io/glfw_window.hpp>

using namespace cubos::core::io;

Window cubos::core::io::openWindow(const std::string& title, const glm::ivec2& size)
{
    return std::make_shared<GLFWWindow>(title, size);
}
