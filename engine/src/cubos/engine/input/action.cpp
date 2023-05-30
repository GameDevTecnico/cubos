#include <cubos/engine/input/action.hpp>

using cubos::core::io::Key;
using cubos::core::io::Modifiers;
using namespace cubos::engine;

const std::vector<std::pair<Key, Modifiers>>& InputAction::keys() const
{
    return mKeys;
}

std::vector<std::pair<Key, Modifiers>>& InputAction::keys()
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
