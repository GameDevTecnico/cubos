#include <memory>
#include <cubos/input/input_manager.hpp>
#include <cubos/input/binding.hpp>

using namespace cubos::io;
using namespace cubos::input;

void InputManager::Init(cubos::io::Window* window)
{
    InputManager::window = window;
}

std::shared_ptr<ActionMapping> InputManager::CreateActionMapping(const std::string& name)
{
    std::shared_ptr<ActionMapping> mapping = std::make_shared<ActionMapping>(name);
    mappings[name] = mapping;

    return mapping;
}

std::shared_ptr<ActionMapping> InputManager::GetActionMapping(const std::string& name)
{
    if (mappings.contains(name))
    {
        // SHould Crash
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

void ActionMapping::SubscribeAction(std::function<void(InputContext)> function)
{
    this->onTrigger.registerCallback(function);
}

void InputManager::RegisterKeyBinding(Key key, std::shared_ptr<Binding> binding)
{
    if (!InputManager::keyBindings.contains(key))
    {
        InputManager::keyBindings[key] = std::vector<std::shared_ptr<Binding>>();
    }
    InputManager::keyBindings[key].push_back(binding);
}

void InputManager::RegisterMouseBinding(MouseButton mouseButton, std::shared_ptr<Binding> binding)
{
    if (!InputManager::mouseButtonsBindings.contains(mouseButton))
    {
        InputManager::mouseButtonsBindings[mouseButton] = std::vector<std::shared_ptr<Binding>>();
    }
    InputManager::mouseButtonsBindings[mouseButton].push_back(binding);
}