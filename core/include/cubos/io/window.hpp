#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <cubos/event.hpp>
#include <cubos/io/keyboard.hpp>

#include <glm/glm.hpp>

namespace cubos::gl
{
    class RenderDevice;
}

namespace cubos::io
{
    /// Mouse button codes enum.
    enum class MouseButton
    {
        Invalid = -1, ///< Used for unknown mouse buttons.

        Left,
        Right,
        Middle,
        Extra1,
        Extra2,
    };

    enum class MouseAxis
    {
        X,
        Y,
        Scroll,
    };

    enum class MouseLockState
    {
        Default, ///< Default lock state, mouse will function as is specified by the OS.
        Locked,  ///< Mouse cursor will be hidden and locked to the center of the window,
                 ///< useful for control schemes that require infinite mouse movement.
        Hidden   ///< Mouse cursor will be hidden when over the window, otherwise functioning in the same way as
                 ///< MouseLockState::Default
    };

    /// Wrapper around a window object.
    /// Handles input events and creates the render device.
    class Window
    {
    public:
        Window() = default;
        virtual ~Window() = default;

        /// Creates a window.
        /// @return New window, or nullptr if creation failed.
        static Window* create();

        /// Polls window events, firing the events.
        virtual void pollEvents() const = 0;

        /// Swaps the window buffers.
        virtual void swapBuffers() const = 0;

        /// Returns the window render device.
        virtual gl::RenderDevice& getRenderDevice() const = 0;

        /// Returns the window framebuffer size in pixels.
        virtual glm::ivec2 getFramebufferSize() const = 0;

        /// Should the window close?
        virtual bool shouldClose() const = 0;

        /// Current time since the window was created.
        virtual double getTime() const = 0;

        /// Set the lock state for the mouse when the window is focused.
        virtual void setMouseLockState(MouseLockState state) = 0;

        /// Invoked with a key code, when a keyboard key is pressed.
        Event<Key> onKeyDown;

        /// Invoked with a key code, when a keyboard key is released.
        Event<Key> onKeyUp;

        /// Invoked with the cursor position when the cursor is moved.
        Event<glm::ivec2> onMouseMoved;

        /// Invoked with the scroll offset, when the mouse wheel is scrolled.
        Event<glm::ivec2> onMouseScroll;

        /// Invoked with a button code when a mouse button is released.
        Event<MouseButton> onMouseUp;

        /// Invoked with a button code when a mouse button is pressed.
        Event<MouseButton> onMouseDown;

        /// Invoked with the new framebuffer size when it changes size.
        Event<glm::ivec2> onFramebufferResize;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
