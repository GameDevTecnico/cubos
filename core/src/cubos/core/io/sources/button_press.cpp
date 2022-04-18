#include <cubos/core/event.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/input_manager.hpp>
#include <cubos/core/io/sources/button_press.hpp>

#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>

using namespace cubos::core::io;

ButtonPress::ButtonPress(cubos::core::io::Key key)
{
    button = key;
}

ButtonPress::ButtonPress(cubos::core::io::MouseButton button)
{
    this->button = button;
}

bool ButtonPress::isTriggered()
{
    if (ButtonPress::wasTriggered)
    {
        ButtonPress::wasTriggered = false;
        return true;
    }
    return false;
}

void ButtonPress::subscribeEvents()
{
    if (std::holds_alternative<cubos::core::io::Key>(this->button))
    {
        InputManager::registerKeyDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                           std::get<cubos::core::io::Key>(this->button));
    }
    else if (std::holds_alternative<cubos::core::io::MouseButton>(this->button))
    {
        InputManager::registerMouseButtonDownCallback<ButtonPress>(
            this, &ButtonPress::handleButtonDown, std::get<cubos::core::io::MouseButton>(this->button));
    }
}

void ButtonPress::unsubscribeEvents()
{
    if (std::holds_alternative<cubos::core::io::Key>(this->button))
    {
        InputManager::unregisterKeyDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                             std::get<cubos::core::io::Key>(this->button));
    }
    else if (std::holds_alternative<cubos::core::io::MouseButton>(this->button))
    {
        InputManager::unregisterMouseButtonDownCallback<ButtonPress>(
            this, &ButtonPress::handleButtonDown, std::get<cubos::core::io::MouseButton>(this->button));
    }
}

void ButtonPress::handleButtonDown()
{
    this->wasTriggered = true;
}

Context ButtonPress::createContext()
{
    return Context();
}
