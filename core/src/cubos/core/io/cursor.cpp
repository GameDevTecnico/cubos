#include <cubos/core/io/cursor.hpp>

using namespace cubos::core;

#ifdef WITH_GLFW
io::Cursor::Cursor(GLFWcursor* handle)
{
    this->glfwHandle = handle;
}
#endif

io::Cursor::~Cursor()
{
#ifdef WITH_GLFW
    if (this->glfwHandle != nullptr)
    {
        glfwDestroyCursor(this->glfwHandle);
    }
#endif
}
