#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <glm/glm.hpp>

namespace cubos::io
{
    // TODO: Format documentation (which style?)
    // Wrapper around a window object, handles input events and creates the render device
    class Window
    {
    public:
        Window(); // TODO: pass settings here?
        ~Window();

        void pollEvents() const;
        void swapBuffers() const;

        glm::ivec2 getFramebufferSize() const;
        bool shouldClose() const;

    private:
        void* handle;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
