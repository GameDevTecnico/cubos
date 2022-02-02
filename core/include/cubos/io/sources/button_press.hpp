#ifndef CUBOS_IO_BUTTON_PRESS_HPP
#define CUBOS_IO_BUTTON_PRESS_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/io/sources/source.hpp>

namespace cubos::io
{

    /// Class use has a handler for button presses
    /// Handles events subscription and context creation for when a button is pressed
    /// Handles button presses from keyboard and mouse
    /// @see Source
    class ButtonPress : public Source
    {
    public:
        std::variant<cubos::io::Key, cubos::io::MouseButton> button;

        /// Creates a Button Press source associated to a keyboard button
        /// @param key the keyboard key associated to this Button Press source
        ButtonPress(cubos::io::Key key);

        /// Creates a Button Press source associated mouse button
        /// @param button the mouse button associated to this Button Press source
        ButtonPress(cubos::io::MouseButton button);

        /// Checks if the button associated with this Button Press source has been pressed
        /// @return true if button associated with this Button Pressed has been pressed, otherwise false
        bool isTriggered() override;

        /// Subscribes this object to callbacks in the Input Manager
        /// @see unsubscribeEvents
        void subscribeEvents() override;

        /// Unsubscribes this object to callbacks in the Input Manager
        /// @see subscribeEvents
        void unsubscribeEvents() override;

        /// Creates the context related to this Button Press
        /// @return context created by this Button Press
        Context createContext() override;

    private:
        bool wasTriggered = false;

        /// Called when the button associated with this Button Press source has been pressed.
        /// Sets the wasTriggered variable to true
        void handleButtonDown();
    };
} // namespace cubos::io
#endif // CUBOS_IO_BUTTON_PRESS_HPP