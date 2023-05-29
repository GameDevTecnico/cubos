/// @file
#include <cubos/core/log.hpp>

#include <cubos/engine/input/axis.hpp>

using namespace cubos::engine;

const std::vector<KeyWithModifiers>& InputAxis::positive() const
{
    return mPositive;
}

const std::vector<KeyWithModifiers>& InputAxis::negative() const
{
    return mNegative;
}

std::vector<KeyWithModifiers>& InputAxis::positive()
{
    return mPositive;
}

std::vector<KeyWithModifiers>& InputAxis::negative()
{
    return mNegative;
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
