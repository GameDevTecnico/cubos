/// @file
#include "cubos/engine/input/action.hpp"

using namespace cubos::engine;

const std::vector<KeyWithModifiers>& InputAction::keys() const
{
    return mKeys;
}

std::vector<KeyWithModifiers>& InputAction::keys()
{
    return mKeys;
}

bool InputAction::pressed() const
{
    return mPressed;
}

void InputAction::pressed(bool pressed)
{
    mPressed = pressed;
}