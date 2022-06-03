#ifndef CUBOS_CORE_IO_WINDOW_HPP
#define CUBOS_CORE_IO_WINDOW_HPP

#include <cubos/core/event.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/cursor.hpp>

#include <glm/glm.hpp>
#include <memory>

namespace cubos::core::gl
{
    class RenderDevice;
}

namespace cubos::core::io
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

    /// IDs of mouse axes.
    enum class MouseAxis
    {
        X,
        Y,
        Scroll,
    };

    /// Possible mouse states.
    enum class MouseState
    {
        Default, ///< Default mouse state, mouse will function as is specified by the OS.
        Locked,  ///< Mouse cursor will be hidden and locked to the center of the window,
                 ///< useful for control schemes that require infinite mouse movement.
        Hidden   ///< Mouse cursor will be hidden when over the window, otherwise functioning in the same way as
                 ///< MouseState::Default
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

        /// Returns the window size, in screen coordinates.
        /// May differ from the framebuffer size in some displays.
        virtual glm::ivec2 getSize() const = 0;

        /// Returns the window framebuffer size in pixels.
        virtual glm::ivec2 getFramebufferSize() const = 0;

        /// Should the window close?
        virtual bool shouldClose() const = 0;

        /// Current time since the window was created.
        virtual double getTime() const = 0;

        /// Set the mouse state when the window is focused.
        virtual void setMouseState(MouseState state) = 0;

        /// Gets the mouse state when the window is focused.
        virtual MouseState getMouseState() const = 0;

        /// Creates a new cursor with a standard shape.
        /// In the future custom cursors should also be supported.
        /// @param standard Standard cursor to use.
        /// @return New cursor, or nullptr if creation failed.
        virtual std::shared_ptr<Cursor> createCursor(Cursor::Standard standard) = 0;

        /// Sets the current cursor. If the cursor is nullptr, the default cursor will be used.
        /// @param cursor Cursor to set.
        virtual void setCursor(std::shared_ptr<Cursor> cursor) = 0;

        /// Sets the content of the clipboard.
        /// @param text Text to set.
        virtual void setClipboard(const std::string& text) = 0;

        /// Gets the content of the clipboard. This string is guaranteed to be valid until the next call to
        /// getClipboard().
        /// @return Text from the clipboard.
        virtual const char* getClipboard() const = 0;

        /// Invoked with a key code, when a keyboard key is pressed.
        Event<Key> onKeyDown;

        /// Invoked with a key code, when a keyboard key is released.
        Event<Key> onKeyUp;

        /// Invoked with the modifiers, when the modifiers change.
        Event<Modifiers> onModsChanged;

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

        /// Invoked when an unicode character is entered.
        Event<char32_t> onChar;
    };
} // namespace cubos::core::io

#endif // CUBOS_CORE_IO_WINDOW_HPP
