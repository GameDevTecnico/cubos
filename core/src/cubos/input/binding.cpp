#include <cubos/input/binding.hpp>

using namespace cubos::input;
using namespace cubos::io;

Binding::Binding(std::shared_ptr<ActionMapping> mapping)
{
    this->mapping = mapping;
}

ButtonBinding::ButtonBinding(std::shared_ptr<ActionMapping> mapping, Key key) : Binding(mapping)
{
    InputManager::RegisterKeyBinding(key, shared_from_this());
}

ButtonBinding::ButtonBinding(std::shared_ptr<ActionMapping> mapping, MouseButton button) : Binding(mapping)
{
    InputManager::RegisterMouseBinding(button, shared_from_this());
}