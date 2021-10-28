#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <cubos/event.hpp>
#include <cubos/io/cubos_key.hpp>

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
        /// Invoked with a CubosKey
        Event<CubosKey> onKeyDown;

        /// Invoked when a keyboard key is released
        /// Invoked with a CubosKey
        Event<CubosKey> onKeyUp;

        /// Invoked when the cursor is moved
        /// Invoked with a vector with x and y positions of the cursor
        Event<glm::ivec2> onMouseMoved;

        /// Invoked when a mouse button is released
        /// Invoked with a MouseSide
        Event<MouseSide> onMouseUp;

        /// Invoked when a mouse button is pressed
        /// Invoked with a MouseSide
        Event<MouseSide> onMouseDown;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
