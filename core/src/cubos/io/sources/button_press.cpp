#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <cubos/io/input_manager.hpp>
#include <cubos/io/sources/button_press.hpp>

#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>

using namespace cubos::io;

ButtonPress::ButtonPress(cubos::io::Key key)
{
    button = key;
}

ButtonPress::ButtonPress(cubos::io::MouseButton button)
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
    if (std::holds_alternative<cubos::io::Key>(this->button))
    {
        InputManager::registerKeyDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                           std::get<cubos::io::Key>(this->button));
    }
    else if (std::holds_alternative<cubos::io::MouseButton>(this->button))
    {
        InputManager::registerMouseButtonDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                                   std::get<cubos::io::MouseButton>(this->button));
    }
}

void ButtonPress::unsubscribeEvents()
{
    if (std::holds_alternative<cubos::io::Key>(this->button))
    {
        InputManager::unregisterKeyDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                             std::get<cubos::io::Key>(this->button));
    }
    else if (std::holds_alternative<cubos::io::MouseButton>(this->button))
    {
        InputManager::unregisterMouseButtonDownCallback<ButtonPress>(this, &ButtonPress::handleButtonDown,
                                                                     std::get<cubos::io::MouseButton>(this->button));
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
