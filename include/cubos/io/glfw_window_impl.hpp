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
        virtual void pollEvents() const override;

        /// Swaps the window buffers
        virtual void swapBuffers() const override;

        /// Return the window framebuffer size in pixels
        virtual glm::ivec2 getFramebufferSize() const override;

        /// Should the window close?
        virtual bool shouldClose() const override;

    private:
        GLFWwindow* handle;
    };
} // namespace cubos::io

#endif // CUBOS_IO_GLFW_WINDOW_IMPL_HPP
