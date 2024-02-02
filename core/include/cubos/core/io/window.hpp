/// @file
/// @brief Class @ref cubos::core::io::Window and related types.
/// @ingroup core-io

#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include <glm/glm.hpp>

#include <cubos/core/io/cursor.hpp>
#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::gl
{
    class RenderDevice;
}

namespace cubos::core::io
{
    class BaseWindow;

    /// @brief Mouse buttons enum.
    /// @ingroup core-io
    enum class MouseButton
    {
        Invalid = -1, ///< Used for unknown mouse buttons.

        Left,
        Right,
        Middle,
        Extra1,
        Extra2,
    };

    /// @brief Mouse axes enums.
    /// @ingroup core-io
    enum class MouseAxis
    {
        X,
        Y,
        Scroll,
    };

    /// @brief Possible mouse states.
    /// @ingroup core-io
    enum class MouseState
    {
        /// @brief Default mouse state, mouse will function as is specified by the OS.
        Default,

        /// @brief Mouse cursor will be hidden and locked to the center of the window, useful for
        /// control schemes that require infinite mouse movement.
        Locked,

        /// @brief Mouse cursor will be hidden when over the window, otherwise functioning in the
        /// same way as MouseState::Default
        Hidden
    };

    /// @brief Event sent when a key is pressed or released.
    /// @ingroup core-io
    struct KeyEvent
    {
        Key key;      ///< Key that was pressed or released.
        bool pressed; ///< Whether the key was pressed or released.
    };

    /// @brief Event sent when the modifiers change.
    /// @ingroup core-io
    struct ModifiersEvent
    {
        Modifiers modifiers; ///< New modifiers.
    };

    /// @brief Event sent when a mouse button state changes.
    /// @ingroup core-io
    struct MouseButtonEvent
    {
        MouseButton button; ///< Button that was pressed or released.
        bool pressed;       ///< Whether the button was pressed or released.
    };

    /// @brief Event sent when the mouse cursor moves.
    /// @ingroup core-io
    struct MouseMoveEvent
    {
        glm::ivec2 position; ///< New position of the mouse cursor.
    };

    /// @brief Event sent when the mouse wheel is scrolled.
    /// @ingroup core-io
    struct MouseScrollEvent
    {
        glm::ivec2 offset; ///< Offset of the scroll.
    };

    /// @brief Event sent when the window framebuffer is resized.
    /// @ingroup core-io
    struct ResizeEvent
    {
        glm::ivec2 size; ///< New size of the framebuffer.
    };

    /// @brief Event sent when a unicode character is input.
    /// @ingroup core-io
    struct TextEvent
    {
        char32_t codepoint; ///< Unicode character that was input.
    };

    /// @brief Event sent when a gamepad is connected or disconnected.
    /// @ingroup core-io
    struct GamepadConnectionEvent
    {
        int gamepad;    ///< Id of the gamepad.
        bool connected; ///< Whether the gamepad was connected.
    };

    /// @brief Variant that can hold any of the window events.
    /// @ingroup core-io
    using WindowEvent = std::variant<KeyEvent, ModifiersEvent, MouseButtonEvent, MouseMoveEvent, MouseScrollEvent,
                                     ResizeEvent, TextEvent, GamepadConnectionEvent>;

    /// @brief Handle to a window.
    /// @see @ref BaseWindow @copybrief BaseWindow
    /// @ingroup core-io
    using Window = std::shared_ptr<BaseWindow>;

    /// @brief Opens a new window.
    /// @param title Window title.
    /// @param size Window size, in screen coordinates.
    /// @return New window, or nullptr on failure.
    /// @ingroup core-io
    Window openWindow(const std::string& title = "CUBOS.", const glm::ivec2& size = {800, 600});

    /// @brief Converts a @ref MouseButton enum to a string.
    /// @param button MouseButton to convert.
    /// @return String representation.
    /// @ingroup core-io
    std::string mouseButtonToString(MouseButton button);

    /// @brief Convert a string to a @ref MouseButton enum.
    /// @param str The string to convert.
    /// @return MouseButton.
    /// @ingroup core-io
    MouseButton stringToMouseButton(const std::string& str);

    /// @brief Interface used to wrap low-level window API implementations.
    ///
    /// Allows polling of input events and creates a @ref gl::RenderDevice for rendering to the
    /// window.
    ///
    /// @ingroup core-io
    class BaseWindow
    {
    public:
        BaseWindow();
        virtual ~BaseWindow() = default;

        /// @brief Pushes an event to the event queue.
        /// @param event Event to push.
        void pushEvent(WindowEvent&& event);

        /// @brief Polls the window for events.
        /// @return Next event, or std::nullopt if there are no more events.
        std::optional<WindowEvent> pollEvent();

        /// @brief Swaps the window buffers.
        virtual void swapBuffers() = 0;

        /// @brief Gets the render device associated with this window.
        /// @return Render device associated with this window.
        virtual gl::RenderDevice& renderDevice() const = 0;

        /// @brief Gets the window size, which may differ from the framebuffer size.
        /// @return Window size, in screen coordinates.
        virtual glm::ivec2 size() const = 0;

        /// @brief Gets the window framebuffer size, which may differ from the window size.
        /// @return Window framebuffer size, in pixels.
        virtual glm::ivec2 framebufferSize() const = 0;

        /// @brief Gets the window content scale, commonly known as "DPI scale".
        /// @return Ratio between the current DPI and the platform's default DPI.
        virtual float contentScale() const = 0;

        /// @brief Checks whether the window should close.
        /// @return Whether the window should close.
        virtual bool shouldClose() const = 0;

        /// @brief Gets the time since the window was created.
        /// @return Time since the window was created, in seconds.
        virtual double time() const = 0;

        /// @brief Sets the mouse state when the window is focused.
        /// @param state Mouse state.
        virtual void mouseState(MouseState state) = 0;

        /// @brief Gets the mouse state when the window is focused.
        /// @return Mouse state when the window is focused.
        virtual MouseState mouseState() const = 0;

        /// @brief Gets the position of the mouse cursor.
        /// @param position Position of the mouse cursor, in screen coordinates.
        virtual glm::ivec2 getMousePosition() = 0;

        /// @brief Sets the position of the mouse cursor.
        /// @param position New position of the mouse cursor, in screen coordinates.
        virtual void setMousePosition(glm::ivec2 position) = 0;

        /// @brief Creates a new cursor with a standard shape.
        /// @todo Custom cursors should also be supported.
        /// @param standard Standard cursor to use.
        /// @return New cursor, or nullptr if creation failed.
        virtual std::shared_ptr<Cursor> createCursor(Cursor::Standard standard) = 0;

        /// @brief Sets the current cursor. Pass nullptr to use the default cursor.
        /// @param cursor New cursor.
        virtual void cursor(std::shared_ptr<Cursor> cursor) = 0;

        /// @brief Sets the content of the clipboard.
        /// @param text New clipboard text.
        virtual void clipboard(const std::string& text) = 0;

        /// @brief Gets the content of the clipboard.
        /// @warning This function is not thread-safe, check #451 for more information.
        /// @return Text from the clipboard. Guaranteed to be valid until the next call.
        virtual const char* clipboard() const = 0;

        /// @brief Gets the last used keyboard modifiers.
        /// @return Active keyboard modifiers.
        virtual Modifiers modifiers() const = 0;

        /// @brief Checks if a key is currently pressed with (at least) the given modifiers.
        /// @param key Key to check.
        /// @param modifiers Modifiers to check.
        /// @return Whether the key and modifiers (or a superset of) are currently pressed.
        virtual bool pressed(Key key, Modifiers modifiers = Modifiers::None) const = 0;

        /// @brief Gets the state of the specified gamepad.
        /// @param gamepad Gamepad to get the state of.
        /// @param state State to fill with the gamepad state.
        /// @return Whether the gamepad was found.
        virtual bool gamepadState(int gamepad, GamepadState& state) const = 0;

    protected:
        /// @brief Asks the implementation to fill the event queue with new events.
        virtual void pollEvents() = 0;

    private:
        bool mPolled;
        std::deque<WindowEvent> mEvents;
    };
} // namespace cubos::core::io

CUBOS_REFLECT_EXTERNAL_DECL(cubos::core::io::MouseButton);
CUBOS_REFLECT_EXTERNAL_DECL(cubos::core::io::MouseAxis);
CUBOS_REFLECT_EXTERNAL_DECL(cubos::core::io::MouseState);
