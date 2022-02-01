#include <memory>
#include <variant>
#include <cubos/io/input_manager.hpp>

using namespace cubos::io;

std::map<std::string, std::shared_ptr<Action>> InputManager::actions = std::map<std::string, std::shared_ptr<Action>>();
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

void InputManager::init(cubos::io::Window* window)
{
    InputManager::window = window;
    InputManager::window->onKeyDown.registerCallback(handleKeyDown);
    // InputManager::window->onKeyUp.registerCallback(handleKeyUp);
    InputManager::window->onMouseDown.registerCallback(handleMouseButtonDown);
    // InputManager::window->onMouseUp.registerCallback(handleMouseUp);
    InputManager::window->onMouseMoved.registerCallback(handleMouseAxis);
}