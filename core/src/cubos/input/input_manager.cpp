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