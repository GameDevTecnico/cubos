#ifndef CUBOS_CORE_IO_BUTTON_PRESS_HPP
#define CUBOS_CORE_IO_BUTTON_PRESS_HPP

#include <cubos/core/event.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/sources/source.hpp>

#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>

namespace cubos::core::io
{

    /// ButtonPress is used to bind button presses to the gameplay logic.
    /// The bindings are not invoked while the button is down.
    /// Handles events subscription and context creation for when a button is pressed
    /// Handles button presses from keyboard and mouse
    /// @see Source
    class ButtonPress : public Source
    {
    public:
        /// Creates a Button Press source associated to a keyboard button
        /// @param key the keyboard key associated to this Button Press source
        ButtonPress(cubos::core::io::Key key);

        /// Creates a Button Press source associated mouse button
        /// @param button the mouse button associated to this Button Press source
        ButtonPress(cubos::core::io::MouseButton button);

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
        std::variant<cubos::core::io::Key, cubos::core::io::MouseButton> button; ///< Button to track

        bool wasTriggered = false;

        /// Called when the button associated with this Button Press source has been pressed.
        /// Sets the wasTriggered variable to true
        void handleButtonDown();
    };
} // namespace cubos::core::io
#endif // CUBOS_CORE_IO_BUTTON_PRESS_HPP
