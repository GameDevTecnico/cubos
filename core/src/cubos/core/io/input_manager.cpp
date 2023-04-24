#include <memory>
#include <variant>

#include <cubos/core/io/input_manager.hpp>

using namespace cubos::core::io;

std::map<std::string, std::shared_ptr<Action>> InputManager::actions = std::map<std::string, std::shared_ptr<Action>>();
std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>> InputManager::keyDownCallbacks =
    std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>>();
std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>> InputManager::keyUpCallbacks =
    std::map<cubos::core::io::Key, std::shared_ptr<cubos::core::Event<>>>();
std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>> InputManager::mouseButtonDownCallbacks =
    std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>>();
std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>> InputManager::mouseButtonUpCallbacks =
    std::map<cubos::core::io::MouseButton, std::shared_ptr<cubos::core::Event<>>>();
std::map<cubos::core::io::MouseAxis, std::shared_ptr<cubos::core::Event<float>>> InputManager::mouseAxisCallbacks =
    std::map<cubos::core::io::MouseAxis, std::shared_ptr<cubos::core::Event<float>>>();

Window InputManager::window = nullptr;

std::shared_ptr<Action> InputManager::createAction(std::string name)
{
    if (InputManager::actions.contains(name))
    {
        // returns null if an action with that name already exists
        return nullptr;
    }

    std::shared_ptr<Action> action = std::make_shared<Action>(name);

    InputManager::actions.insert({name, action});
    return action;
};

std::shared_ptr<Action> InputManager::getAction(std::string name)
{
    if (InputManager::actions.contains(name))
        return InputManager::actions[name];
    return nullptr;
};

void InputManager::processActions()
{
    for (auto it = InputManager::actions.begin(); it != InputManager::actions.end(); it++)
    {
        it->second->processSources();
    }
}

void InputManager::handleKeyDown(cubos::core::io::Key key)
{
    if (keyDownCallbacks.contains(key))
    {
        keyDownCallbacks[key]->fire();
    }
};

void InputManager::handleKeyUp(cubos::core::io::Key key)
{
    if (keyUpCallbacks.contains(key))
    {
        keyUpCallbacks[key]->fire();
    }
}

void InputManager::handleMouseButtonDown(cubos::core::io::MouseButton mouseButton)
{
    if (mouseButtonDownCallbacks.contains(mouseButton))
    {
        mouseButtonDownCallbacks[mouseButton]->fire();
    }
};

void InputManager::handleMouseButtonUp(cubos::core::io::MouseButton mouseButton)
{
    if (mouseButtonUpCallbacks.contains(mouseButton))
    {
        mouseButtonUpCallbacks[mouseButton]->fire();
    }
};

void InputManager::handleMouseAxis(glm::ivec2 coordinates)
{
    if (mouseAxisCallbacks.contains(cubos::core::io::MouseAxis::X))
    {
        mouseAxisCallbacks[cubos::core::io::MouseAxis::X]->fire(float(coordinates.x));
    }

    if (mouseAxisCallbacks.contains(cubos::core::io::MouseAxis::Y))
    {
        mouseAxisCallbacks[cubos::core::io::MouseAxis::Y]->fire(float(coordinates.y));
    }
}

void InputManager::init(Window window)
{
    InputManager::window = window;
    InputManager::window->onKeyDown.registerCallback(handleKeyDown);
    InputManager::window->onKeyUp.registerCallback(handleKeyUp);
    InputManager::window->onMouseDown.registerCallback(handleMouseButtonDown);
    // InputManager::window->onMouseUp.registerCallback(handleMouseUp);
    InputManager::window->onMouseMoved.registerCallback(handleMouseAxis);
}

cubos::core::Event<>::ID InputManager::registerKeyDownCallback(std::function<void(void)> callback,
                                                               cubos::core::io::Key key)
{
    if (!InputManager::keyDownCallbacks.contains(key))
    {
        InputManager::keyDownCallbacks[key] = std::make_shared<cubos::core::Event<>>();
    }
    return InputManager::keyDownCallbacks[key]->registerCallback(callback);
}

void InputManager::unregisterKeyDownCallback(cubos::core::Event<>::ID callbackID, cubos::core::io::Key key)
{
    if (!InputManager::keyDownCallbacks.contains(key))
    {
        return;
    }
    InputManager::keyDownCallbacks[key]->unregisterCallback(callbackID);
}

cubos::core::Event<>::ID InputManager::registerKeyUpCallback(std::function<void(void)> callback,
                                                             cubos::core::io::Key key)
{
    if (!InputManager::keyUpCallbacks.contains(key))
    {
        InputManager::keyUpCallbacks[key] = std::make_shared<cubos::core::Event<>>();
    }
    return InputManager::keyUpCallbacks[key]->registerCallback(callback);
}

void InputManager::unregisterKeyUpCallback(cubos::core::Event<>::ID callbackID, cubos::core::io::Key key)
{
    if (!InputManager::keyUpCallbacks.contains(key))
    {
        return;
    }
    InputManager::keyUpCallbacks[key]->unregisterCallback(callbackID);
}

cubos::core::Event<>::ID InputManager::registerMouseButtonDownCallback(std::function<void(void)> callback,
                                                                       cubos::core::io::MouseButton mouseButton)
{
    if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
    {
        InputManager::mouseButtonDownCallbacks[mouseButton] = std::make_shared<cubos::core::Event<>>();
    }
    return InputManager::mouseButtonDownCallbacks[mouseButton]->registerCallback(callback);
}

void InputManager::unregisterMouseButtonDownCallback(cubos::core::Event<>::ID callbackID,
                                                     cubos::core::io::MouseButton mouseButton)
{
    if (!InputManager::mouseButtonDownCallbacks.contains(mouseButton))
    {
        return;
    }
    InputManager::mouseButtonDownCallbacks[mouseButton]->unregisterCallback(callbackID);
}

cubos::core::Event<>::ID InputManager::registerMouseButtonUpCallback(std::function<void(void)> callback,
                                                                     cubos::core::io::MouseButton mouseButton)
{
    if (!InputManager::mouseButtonUpCallbacks.contains(mouseButton))
    {
        InputManager::mouseButtonUpCallbacks[mouseButton] = std::make_shared<cubos::core::Event<>>();
    }
    return InputManager::mouseButtonUpCallbacks[mouseButton]->registerCallback(callback);
}

void InputManager::unregisterMouseButtonUpCallback(cubos::core::Event<>::ID callbackID,
                                                   cubos::core::io::MouseButton mouseButton)
{
    if (!InputManager::mouseButtonUpCallbacks.contains(mouseButton))
    {
        return;
    }
    InputManager::mouseButtonUpCallbacks[mouseButton]->unregisterCallback(callbackID);
}

cubos::core::Event<float>::ID InputManager::registerMouseAxisCallback(std::function<void(float)> callback,
                                                                      cubos::core::io::MouseAxis mouseAxis)
{
    if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
    {
        InputManager::mouseAxisCallbacks[mouseAxis] = std::make_shared<cubos::core::Event<float>>();
    }
    return InputManager::mouseAxisCallbacks[mouseAxis]->registerCallback(callback);
}

void InputManager::unregisterMouseAxisCallback(cubos::core::Event<float>::ID callbackID,
                                               cubos::core::io::MouseAxis mouseAxis)
{
    if (!InputManager::mouseAxisCallbacks.contains(mouseAxis))
    {
        return;
    }
    InputManager::mouseAxisCallbacks[mouseAxis]->unregisterCallback(callbackID);
}
