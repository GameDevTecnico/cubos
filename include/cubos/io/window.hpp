#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <cubos/event.hpp>

#include <glm/glm.hpp>

namespace cubos::gl
{
    class RenderDevice;
}

namespace cubos::io
{
    enum class MouseSide
    {
        Left,
        Right
    };

    /// Wrapper around a window object, handles input events and creates the render device
    class Window
    {
    public:
        Window() = default;
        virtual ~Window() = default;

        /// Polls window events, firing the events
        virtual void pollEvents() const = 0;

        /// Swaps the window buffers
        virtual void swapBuffers() const = 0;

        /// Returns the window render device
        virtual gl::RenderDevice& getRenderDevice() const = 0;

        /// Return the window framebuffer size in pixels
        virtual glm::ivec2 getFramebufferSize() const = 0;

        /// Should the window close?
        virtual bool shouldClose() const = 0;

        /// Invoked when a keyboard key is pressed
        /// Returns GLFW key input
        Event<int> onKeyDown;

        /// Invoked when a keyboard key is released
        /// Returns GLFW key input
        Event<int> onKeyUp;

        /// Invoked when the cursor is moved
        /// Returns vector with x and y positions of the cursor
        Event<glm::ivec2> onMouseMoved;

        /// Invoked when the mouse right click is released
        /// Returns nothing
        Event<MouseSide> onMouseUp;

        /// Invoked when the mouse right click is pressed
        /// Returns nothing
        Event<MouseSide> onMouseDown;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
