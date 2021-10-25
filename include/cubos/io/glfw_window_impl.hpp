#ifndef CUBOS_IO_GLFW_WINDOW_IMPL_HPP
#define CUBOS_IO_GLFW_WINDOW_IMPL_HPP

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <cubos/io/window.hpp>

namespace cubos::io
{
    /// Wrapper around a window object, handles input events and creates the render device
    class GLFWWindowImpl : public cubos::io::Window
    {
    public:
        GLFWWindowImpl(); // TODO: pass settings here?
        ~GLFWWindowImpl();

        /// Polls window events, firing the events
        void pollEvents() const;

        /// Swaps the window buffers
        void swapBuffers() const;

        /// Return the window framebuffer size in pixels
        glm::ivec2 getFramebufferSize() const;

        /// Should the window close?
        bool shouldClose() const;

    private:
        GLFWwindow* handle;
    };
} // namespace cubos::io::glfw

#endif // CUBOS_IO_GLFW_WINDOW_IMPL_HPP
