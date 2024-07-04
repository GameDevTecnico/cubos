#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/action.hpp>

using cubos::core::io::GamepadButton;
using cubos::core::io::Key;
using cubos::core::io::MouseButton;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputAction)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAction")
        .with(FieldsTrait{}
                  .withField("keys", &InputAction::mKeys)
                  .withField("gamepadButtons", &InputAction::mGamepadButtons)
                  .withField("mouseButtons", &InputAction::mMouseButtons));
}

const std::vector<Key>& InputAction::keys() const
{
    return mKeys;
}

const std::vector<GamepadButton>& InputAction::gamepadButtons() const
{
    return mGamepadButtons;
}

const std::vector<MouseButton>& InputAction::mouseButtons() const
{
    return mMouseButtons;
}

std::vector<Key>& InputAction::keys()
{
    return mKeys;
}

std::vector<GamepadButton>& InputAction::gamepadButtons()
{
    return mGamepadButtons;
}

std::vector<MouseButton>& InputAction::mouseButtons()
{
    return mMouseButtons;
}

void InputAction::pressed(bool pressed)
{
    mPressed = pressed;
}

void InputAction::justPressed(bool justPressed)
{
    mJustPressed = justPressed;
}

void InputAction::justReleased(bool justReleased)
{
    mJustReleased = justReleased;
}

bool InputAction::pressed() const
{
    return mPressed;
}

bool InputAction::justPressed() const
{
    return mJustPressed;
}

bool InputAction::justReleased() const
{
    return mJustReleased;
}
