#include <cubos/core/io/cursor.hpp>

using namespace cubos::core;

#ifdef WITH_GLFW
io::Cursor::Cursor(GLFWcursor* handle)
{
    mGlfwHandle = handle;
}
#endif

io::Cursor::~Cursor()
{
#ifdef WITH_GLFW
    if (mGlfwHandle != nullptr)
    {
        glfwDestroyCursor(mGlfwHandle);
    }
#endif
}
