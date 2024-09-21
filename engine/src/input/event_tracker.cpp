#include <cubos/engine/input/event_tracker.hpp>

using namespace cubos::engine;

using cubos::core::io::GamepadAxis;
using cubos::core::io::GamepadButton;
using cubos::core::io::Key;
using cubos::core::io::MouseAxis;
using cubos::core::io::MouseButton;

void InputEventTracker::listen(Key key)
{
    mKeysListening.set(static_cast<size_t>(key));
}

void InputEventTracker::listen(MouseButton button)
{
    mButtonsListening.set(static_cast<size_t>(button));
}

void InputEventTracker::listen(GamepadButton button)
{
    mButtonsListening.set(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(button));
}

void InputEventTracker::listen(MouseAxis axis)
{
    mAxesListening.set(static_cast<size_t>(axis));
}

void InputEventTracker::listen(GamepadAxis axis)
{
    mAxesListening.set(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(axis));
}

void InputEventTracker::ignore(Key key)
{
    mKeysListening.reset(static_cast<size_t>(key));
}

void InputEventTracker::ignore(MouseButton button)
{
    mButtonsListening.reset(static_cast<size_t>(button));
}

void InputEventTracker::ignore(GamepadButton button)
{
    mButtonsListening.reset(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(button));
}

void InputEventTracker::ignore(MouseAxis axis)
{
    mAxesListening.reset(static_cast<size_t>(axis));
}

void InputEventTracker::ignore(GamepadAxis axis)
{
    mAxesListening.reset(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(axis));
}

bool InputEventTracker::trigger(Key key)
{
    if (mKeysListening.test(static_cast<size_t>(key)))
    {
        mKeysTriggered.set(static_cast<size_t>(key));
        return true;
    }

    return false;
}

bool InputEventTracker::trigger(MouseButton button)
{
    if (mButtonsListening.test(static_cast<size_t>(button)))
    {
        mButtonsTriggered.set(static_cast<size_t>(button));
        return true;
    }

    return false;
}

bool InputEventTracker::trigger(GamepadButton button)
{
    if (mButtonsListening.test(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(button)))
    {
        mButtonsTriggered.set(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(button));
        return true;
    }

    return false;
}

bool InputEventTracker::trigger(MouseAxis axis)
{
    if (mAxesListening.test(static_cast<size_t>(axis)))
    {
        mAxesTriggered.set(static_cast<size_t>(axis));
        return true;
    }

    return false;
}

bool InputEventTracker::trigger(GamepadAxis axis)
{
    if (mAxesListening.test(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(axis)))
    {
        mAxesTriggered.set(static_cast<size_t>(MouseButton::Count) + static_cast<size_t>(axis));
        return true;
    }

    return false;
}

const std::bitset<InputEventTracker::KeyCount>& InputEventTracker::keysTriggered() const
{
    return mKeysTriggered;
}

const std::bitset<InputEventTracker::ButtonCount>& InputEventTracker::buttonsTriggered() const
{
    return mButtonsTriggered;
}

const std::bitset<InputEventTracker::AxisCount>& InputEventTracker::axesTriggered() const
{
    return mAxesTriggered;
}
