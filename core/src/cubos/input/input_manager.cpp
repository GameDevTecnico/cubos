#include <memory>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/binding.hpp>

using namespace cubos::io;
using namespace cubos::input;

std::map<std::string, std::shared_ptr<InputAction>> InputManager::actions =
    std::map<std::string, std::shared_ptr<InputAction>>();
std::map<cubos::io::Key, std::vector<std::function<void(void)>>> InputManager::keyDownCallbacks =
    std::map<cubos::io::Key, std::vector<std::function<void(void)>>>();
std::map<cubos::io::Key, std::vector<std::function<void(void)>>> InputManager::keyUpCallbacks =
    std::map<cubos::io::Key, std::vector<std::function<void(void)>>>();
cubos::io::Window* InputManager::window = nullptr;

void InputManager::registerKeyDownCallback(std::function<void()> callback, cubos::io::Key key)
{
    if (InputManager::keyDownCallbacks.contains(key))
    {
        InputManager::keyDownCallbacks[key].push_back(callback);
    }
    else
    {
        std::vector<std::function<void(void)>> v = std::vector<std::function<void(void)>>();
        v.push_back(callback);
        InputManager::keyDownCallbacks.insert({key, v});
    }
};

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
    InputManager::registerKeyDownCallback([this]() { this->handleKeyDown(); }, std::get<cubos::io::Key>(this->button));
}

void ButtonPress::unsubscribeEvents(cubos::io::Window* window)
{
}

void ButtonPress::handleKeyDown()
{
    this->wasTriggered = true;
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
    for (auto itSource = InputAction::inputSources.begin(); itSource != InputAction::inputSources.end(); itSource++)
    {
        if ((*itSource)->isTriggered())
        {
            for (auto itBinding = InputAction::functionBindings.begin();
                 itBinding != InputAction::functionBindings.end(); itBinding++)
            {
                (*itBinding)(InputContext());
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
    for (auto itCallbacksVector = InputManager::keyDownCallbacks.begin();
         itCallbacksVector != InputManager::keyDownCallbacks.end(); itCallbacksVector++)
    {
        if (itCallbacksVector->first == key)
        {
            for (auto itCallback = itCallbacksVector->second.begin(); itCallback != itCallbacksVector->second.end();
                 itCallback++)
            {
                (*itCallback)();
            }
        }
    };
};

void InputManager::init(cubos::io::Window* window)
{
    InputManager::window = window;
    InputManager::window->onKeyDown.registerCallback(handleKeyDown);
    // InputManager::window->onKeyUp.registerCallback(handleKeyUp);
    // InputManager::window->onMouseDown.registerCallback(handleMouseDown);
    // InputManager::window->onMouseUp.registerCallback(handleMouseUp);
}