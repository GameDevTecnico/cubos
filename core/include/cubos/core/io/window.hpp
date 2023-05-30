#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include <glm/glm.hpp>

#include <cubos/core/io/cursor.hpp>
#include <cubos/core/io/keyboard.hpp>

namespace cubos::core::gl
{
    class RenderDevice;
}

namespace cubos::core::io
{
    class BaseWindow;

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

    /// Event sent when a key is pressed or released.
    struct KeyEvent
    {
        Key key;      ///< Key that was pressed or released.
        bool pressed; ///< True if the key was pressed, false if it was released.
    };

    /// Event sent when the modifiers change.
    struct ModifiersEvent
    {
        Modifiers modifiers; ///< New modifiers.
    };

    /// Event sent when a mouse button state changes.
    struct MouseButtonEvent
    {
        MouseButton button; ///< Button that was pressed or released.
        bool pressed;       ///< True if the button was pressed, false if it was released.
    };

    /// Event sent when the mouse cursor moves.
    struct MouseMoveEvent
    {
        glm::ivec2 position; ///< New position of the mouse cursor.
    };

    /// Event sent when the mouse wheel is scrolled.
    struct MouseScrollEvent
    {
        glm::ivec2 offset; ///< Offset of the scroll.
    };

    /// Event sent when the window framebuffer is resized.
    struct ResizeEvent
    {
        glm::ivec2 size; ///< New size of the framebuffer.
    };

    /// Event sent when a unicode character is input.
    struct TextEvent
    {
        char32_t codepoint; ///< Unicode character that was input.
    };

    /// Variant that can hold any of the window events.
    using WindowEvent = std::variant<KeyEvent, ModifiersEvent, MouseButtonEvent, MouseMoveEvent, MouseScrollEvent,
                                     ResizeEvent, TextEvent>;

    /// Handle to a generic window.
    using Window = std::shared_ptr<BaseWindow>;

    /// Opens a new window.
    /// @param title Window title.
    /// @param size Window size, in screen coordinates.
    /// @return New window, or nullptr if creation failed.
    Window openWindow(const std::string& title = "Cubos", const glm::ivec2& size = {800, 600});

    /// The base class for all window implementations.
    /// Handles input events and creates the render device.
    class BaseWindow
    {
    public:
        BaseWindow();
        virtual ~BaseWindow() = default;

        /// Pushes an event to the event queue.
        /// @param event Event to push.
        void pushEvent(WindowEvent&& event);

        /// Polls the window for events.
        /// @return The next event, or std::nullopt if there are no more events.
        std::optional<WindowEvent> pollEvent();

        /// Swaps the window buffers.
        virtual void swapBuffers() = 0;

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

        /// @return Whether the key is currently pressed.
        virtual bool keyPressed(Key key) const = 0;

        /// @return Whether the key and modifiers are currently pressed.
        virtual bool keyPressed(Key key, Modifiers modifiers) const = 0;

    protected:
        /// Asks the implementation to fill the event queue with new events.
        virtual void pollEvents() = 0;

    private:
        bool mPolled;
        std::deque<WindowEvent> mEvents;
    };
} // namespace cubos::core::io
