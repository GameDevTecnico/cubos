#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/input_sources/button_press.hpp>

using namespace cubos::input;

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

void ButtonPress::subscribeEvents(cubos::io::Window* window)
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

void ButtonPress::unsubscribeEvents(cubos::io::Window* window)
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

InputContext ButtonPress::createInputContext()
{
    return InputContext();
}