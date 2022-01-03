#include <memory>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/binding.hpp>

using namespace cubos::io;
using namespace cubos::input;

std::map<std::string, std::shared_ptr<InputAction>> InputManager::actions =
    std::map<std::string, std::shared_ptr<InputAction>>();
cubos::io::Window* InputManager::window = nullptr;
std::shared_ptr<InputAction> InputManager::CreateAction(std::string name)
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

std::shared_ptr<InputAction> InputManager::GetAction(std::string name)
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

void ButtonPress::SubscribeEvents(cubos::io::Window* window)
{
    window->onKeyDown.registerCallback([this](cubos::io::Key key) { this->handleKeyDown(key); });
}

void ButtonPress::UnsubscribeEvents(cubos::io::Window* window)
{
}

void ButtonPress::handleKeyDown(cubos::io::Key key)
{
    // verify the key
    this->wasTriggered = true;
}

void InputAction::AddInput(InputSource* source)
{
    source->SubscribeEvents(InputManager::window);
    this->inputSources.push_back(source);
}

void InputAction::AddBinding(std::function<void(InputContext)> binding)
{
    this->functionBindings.push_back(binding);
}

void InputAction::ProcessSources()
{
    std::list<InputSource*>::iterator itSource;
    std::list<std::function<void(InputContext)>>::iterator itBinding;
    for (itSource = InputAction::inputSources.begin(); itSource != InputAction::inputSources.end(); itSource++)
    {
        if ((*itSource)->isTriggered())
        {
            for (itBinding = InputAction::functionBindings.begin(); itBinding != InputAction::functionBindings.end();
                 itBinding++)
            {
                (*itBinding)(InputContext());
            }
            return;
        }
    }
}

void InputManager::ProcessActions()
{
    std::map<std::string, std::shared_ptr<InputAction>>::iterator it;
    for (it = InputManager::actions.begin(); it != InputManager::actions.end(); it++)
    {
        it->second->ProcessSources();
    }
}

void InputManager::Init(cubos::io::Window* window)
{
    InputManager::window = window;
}

/*
std::map<std::string, std::shared_ptr<ActionMapping>> InputManager::mappings =
    std::map<std::string, std::shared_ptr<ActionMapping>>();
std::map<cubos::io::Key, std::vector<std::function<void(void)>>> InputManager::keyBindings =
    std::map<cubos::io::Key, std::vector<std::function<void(void)>>>();
std::map<cubos::io::MouseButton, std::vector<std::function<void(void)>>> InputManager::mouseButtonsBindings =
    std::map<cubos::io::MouseButton, std::vector<std::function<void(void)>>>();
cubos::io::Window* InputManager::window = nullptr;
ActionMapping::ActionMapping(const std::string& actionId)
{
}

void InputManager::Init(cubos::io::Window* window)
{
    InputManager::window = window;
    InputManager::window->onKeyDown.registerCallback(handleKeyDown);
    InputManager::window->onKeyUp.registerCallback(handleKeyUp);
    InputManager::window->onMouseDown.registerCallback(handleMouseDown);
    InputManager::window->onMouseUp.registerCallback(handleMouseUp);
}

void InputManager::handleKeyDown(Key key)
{
    if (InputManager::keyBindings.empty() || !InputManager::keyBindings.contains(key) ||
        InputManager::keyBindings[key].empty())
    {
        return;
    }

    for (std::vector<std::function<void(void)>>::iterator it = InputManager::keyBindings[key].begin();
         it != InputManager::keyBindings[key].end(); it++)
    {
        (*it)(); // TODO: Share state of the key binding (was it pressed)
    }
}

void InputManager::handleKeyUp(Key key)
{
    if (InputManager::keyBindings.empty() || !InputManager::keyBindings.contains(key) ||
        InputManager::keyBindings[key].empty())
    {
        return;
    }

    for (std::vector<std::function<void(void)>>::iterator it = InputManager::keyBindings[key].begin();
         it != InputManager::keyBindings[key].end(); it++)
    {
        (*it)(); // TODO: Share state of the key binding (was it pressed)
    }
}

std::shared_ptr<ActionMapping> InputManager::CreateActionMapping(const std::string& name)
{
    std::shared_ptr<ActionMapping> mapping = std::make_shared<ActionMapping>(name);
    mappings[name] = mapping;

    return mapping;
}

std::shared_ptr<ActionMapping> InputManager::GetActionMapping(const std::string& name)
{
    if (!mappings.contains(name))
    {
        // Should Crash
    }

    return mappings[name];
}

void ActionMapping::RegisterBinding(ButtonBindingDesc bindingDesc)
{
    std::shared_ptr<Binding> binding;

    switch (bindingDesc.type)
    {
    case ButtonBindingDesc::ButtonType::Key:
        binding = std::make_shared<ButtonBinding>(shared_from_this(), std::get<Key>(bindingDesc.button));
        break;
    case ButtonBindingDesc::ButtonType::Mouse:
        binding = std::make_shared<ButtonBinding>(shared_from_this(), std::get<MouseButton>(bindingDesc.button));
        break;
    default:
        break;
    }

    this->bindings.push_back(binding);
}

void InputManager::handleMouseDown(MouseButton mouseButton)
{
    if (InputManager::mouseButtonsBindings.empty() || !InputManager::mouseButtonsBindings.contains(mouseButton) ||
        InputManager::mouseButtonsBindings[mouseButton].empty())
    {
        return;
    }

    for (std::vector<std::function<void(void)>>::iterator it = InputManager::mouseButtonsBindings[mouseButton].begin();
         it != InputManager::mouseButtonsBindings[mouseButton].end(); it++)
    {
        (*it)(); // TODO: Share state of the key binding (was it pressed)
    }
}

void InputManager::handleMouseUp(MouseButton mouseButton)
{
    if (InputManager::mouseButtonsBindings.empty() || !InputManager::mouseButtonsBindings.contains(mouseButton) ||
        InputManager::mouseButtonsBindings[mouseButton].empty())
    {
        return;
    }

    for (std::vector<std::function<void(void)>>::iterator it = InputManager::mouseButtonsBindings[mouseButton].begin();
         it != InputManager::mouseButtonsBindings[mouseButton].end(); it++)
    {
        (*it)(); // TODO: Share state of the key binding (was it pressed)
    }
}

void ActionMapping::SubscribeAction(std::function<void(InputContext)> function)
{
    this->onTrigger.registerCallback(function);
}

void InputManager::RegisterKeyBinding(Key key, std::function<void(void)> callback)
{
    if (!InputManager::keyBindings.contains(key))
    {
        InputManager::keyBindings[key] = std::vector<std::function<void(void)>>();
    }
    InputManager::keyBindings[key].push_back(callback);
}

void InputManager::RegisterMouseBinding(MouseButton mouseButton, std::function<void(void)> callback)
{
    if (!InputManager::mouseButtonsBindings.contains(mouseButton))
    {
        InputManager::mouseButtonsBindings[mouseButton] = std::vector<std::function<void(void)>>();
    }
    InputManager::mouseButtonsBindings[mouseButton].push_back(callback);
}

void InputManager::RegisterAxisBinding(cubos::io::Key keyPos, cubos::io::Key keyNeg,
                                       std::function<void(float)> callback)
{
    if (!InputManager::keyBindings.contains(keyPos))
    {
        InputManager::keyBindings[keyPos] = std::vector<std::function<void(void)>>();
    }
    InputManager::keyBindings[keyPos].push_back([&]() { callback(1); });

    if (!InputManager::keyBindings.contains(keyNeg))
    {
        InputManager::keyBindings[keyNeg] = std::vector<std::function<void(void)>>();
    }
    InputManager::keyBindings[keyNeg].push_back([&]() { callback(-1); });
}*/