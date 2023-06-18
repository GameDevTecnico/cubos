#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/axis.hpp>

using cubos::core::io::GamepadAxis;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;
using namespace cubos::engine;

const std::vector<std::pair<Key, Modifiers>>& InputAxis::positive() const
{
    return mPositive;
}

const std::vector<std::pair<Key, Modifiers>>& InputAxis::negative() const
{
    return mNegative;
}

const std::vector<GamepadAxis>& InputAxis::gamepadAxes() const
{
    return mGamepadAxes;
}

std::vector<std::pair<Key, Modifiers>>& InputAxis::positive()
{
    return mPositive;
}

std::vector<std::pair<Key, Modifiers>>& InputAxis::negative()
{
    return mNegative;
}

std::vector<GamepadAxis>& InputAxis::gamepadAxes()
{
    return mGamepadAxes;
}

float InputAxis::value() const
{
    return mValue;
}

void InputAxis::value(float value)
{
    if (std::abs(value) > 1.0f)
    {
        CUBOS_WARN("Axis value out of range: {}", value);
    }
    mValue = std::clamp(value, -1.0f, 1.0f);
}
