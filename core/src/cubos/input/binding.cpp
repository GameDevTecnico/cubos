#include <cubos/input/binding.hpp>

using namespace cubos::input;
using namespace cubos::io;

Binding::Binding(std::shared_ptr<ActionMapping> mapping)
{
    this->mapping = mapping;
}

ButtonBinding::ButtonBinding(std::shared_ptr<ActionMapping> mapping, Key key) : Binding(mapping)
{
    InputManager::RegisterKeyBinding(key, std::bind(&ButtonBinding::handleButtonPress, this));
}

ButtonBinding::ButtonBinding(std::shared_ptr<ActionMapping> mapping, MouseButton button) : Binding(mapping)
{
    InputManager::RegisterMouseBinding(button, std::bind(&ButtonBinding::handleButtonPress, this));
}

void ButtonBinding::handleButtonPress()
{
    // TODO
}

AxisBinding::AxisBinding(std::shared_ptr<ActionMapping> mapping, Key keyPos, Key keyNeg) : Binding(mapping)
{
    InputManager::RegisterKeyBinding(keyPos, std::bind(&AxisBinding::handlePosKey, this));
    InputManager::RegisterKeyBinding(keyNeg, std::bind(&AxisBinding::handleNegKey, this));
}

void AxisBinding::handlePosKey()
{
    // TODO
}

void AxisBinding::handleNegKey()
{
    // TODO
}