#include <memory>
#include <variant>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/binding.hpp>

using namespace cubos::io;
using namespace cubos::input;

std::map<std::string, std::shared_ptr<InputAction>> InputManager::actions =
    std::map<std::string, std::shared_ptr<InputAction>>();
std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>> InputManager::keyDownCallbacks =
    std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>>();
std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>> InputManager::keyUpCallbacks =
    std::map<cubos::io::Key, std::shared_ptr<cubos::Event<>>>();
std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>> InputManager::mouseButtonDownCallbacks =
    std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>>();
std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>> InputManager::mouseButtonUpCallbacks =
    std::map<cubos::io::MouseButton, std::shared_ptr<cubos::Event<>>>();
std::map<cubos::io::MouseAxis, std::shared_ptr<cubos::Event<float>>> InputManager::mouseAxisCallbacks =
    std::map<cubos::io::MouseAxis, std::shared_ptr<cubos::Event<float>>>();

cubos::io::Window* InputManager::window = nullptr;

std::shared_ptr<InputAction> InputManager::createAction(std::string name)
{
    if (InputManager::actions.contains(name))
    {
        // returns null if an action with that name already exists
        return nullptr;
    }

    std::shared_ptr<InputAction> action = std::make_shared<InputAction>(name);

    InputManager::actions.insert({name, action});
    return action;
};

std::shared_ptr<InputAction> InputManager::getAction(std::string name)
{
    if (InputManager::actions.contains(name))
        return InputManager::actions[name];
    return nullptr;
};

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

void InputAction::addInput(InputSource* source)
{
    source->subscribeEvents(InputManager::window);
    this->inputSources.push_back(source);
}

void InputAction::addBinding(std::function<void(InputContext)> binding)
{
    this->functionBindings.push_back(binding);
}

void InputAction::processSources()
{
    for (auto itSource = this->inputSources.begin(); itSource != this->inputSources.end(); itSource++)
    {
        if ((*itSource)->isTriggered())
        {
            for (auto itBinding = this->functionBindings.begin(); itBinding != this->functionBindings.end();
                 itBinding++)
            {
                (*itBinding)((*itSource)->createInputContext());
            }
            return;
        }
    }
}

void InputManager::processActions()
{
    for (auto it = InputManager::actions.begin(); it != InputManager::actions.end(); it++)
    {
        it->second->processSources();
    }
}

void InputManager::handleKeyDown(cubos::io::Key key)
{
    if (keyDownCallbacks.contains(key))
    {
        keyDownCallbacks[key]->fire();
    }
};

void InputManager::handleMouseButtonDown(cubos::io::MouseButton mouseButton)
{
    if (mouseButtonDownCallbacks.contains(mouseButton))
    {
        mouseButtonDownCallbacks[mouseButton]->fire();
    }
};

void InputManager::handleMouseAxis(glm::ivec2 coordinates)
{
    if (mouseAxisCallbacks.contains(cubos::io::MouseAxis::X))
    {
        mouseAxisCallbacks[cubos::io::MouseAxis::X]->fire(float(coordinates.x));
    }

    if (mouseAxisCallbacks.contains(cubos::io::MouseAxis::Y))
    {
        mouseAxisCallbacks[cubos::io::MouseAxis::Y]->fire(float(coordinates.y));
    }
}

void DoubleAxis::subscribeEvents(cubos::io::Window* window)
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->horizontalAxis))
    {
        InputManager::registerMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleHorizontalAxis,
                                                            std::get<cubos::io::MouseAxis>(this->horizontalAxis));
    }

    if (std::holds_alternative<cubos::io::MouseAxis>(this->verticalAxis))
    {
        InputManager::registerMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleVerticalAxis,
                                                            std::get<cubos::io::MouseAxis>(this->verticalAxis));
    }
};

void DoubleAxis::unsubscribeEvents(cubos::io::Window* window)
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->horizontalAxis))
    {
        InputManager::unregisterMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleHorizontalAxis,
                                                              std::get<cubos::io::MouseAxis>(this->horizontalAxis));
    }

    if (std::holds_alternative<cubos::io::MouseAxis>(this->verticalAxis))
    {
        InputManager::unregisterMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleVerticalAxis,
                                                              std::get<cubos::io::MouseAxis>(this->verticalAxis));
    }
};

void DoubleAxis::handleHorizontalAxis(float xPos)
{
    this->xPos = xPos;
    this->wasTriggered = true;
}

void DoubleAxis::handleVerticalAxis(float yPos)
{
    this->yPos = yPos;
    this->wasTriggered = true;
}

bool DoubleAxis::isTriggered()
{
    if (DoubleAxis::wasTriggered)
    {
        DoubleAxis::wasTriggered = false;
        return true;
    }
    return false;
}

InputContext DoubleAxis::createInputContext()
{
    return InputContext(glm::vec2(this->xPos, this->yPos));
}

glm::vec2 InputContext::getValue()
{
    return this->value;
}

void InputManager::init(cubos::io::Window* window)
{
    InputManager::window = window;
    InputManager::window->onKeyDown.registerCallback(handleKeyDown);
    // InputManager::window->onKeyUp.registerCallback(handleKeyUp);
    InputManager::window->onMouseDown.registerCallback(handleMouseButtonDown);
    // InputManager::window->onMouseUp.registerCallback(handleMouseUp);
    InputManager::window->onMouseMoved.registerCallback(handleMouseAxis);
}