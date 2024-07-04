#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/combination.hpp>

using cubos::core::io::GamepadButton;
using cubos::core::io::GamepadState;
using cubos::core::io::Key;
using cubos::core::io::MouseButton;
using cubos::core::io::Window;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputCombination)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputCombination")
        .with(FieldsTrait{}
                  .withField("keys", &InputCombination::mKeys)
                  .withField("gamepadButtons", &InputCombination::mGamepadButtons)
                  .withField("mouseButtons", &InputCombination::mMouseButtons));
}

const std::vector<Key>& InputCombination::keys() const
{
    return mKeys;
}

const std::vector<GamepadButton>& InputCombination::gamepadButtons() const
{
    return mGamepadButtons;
}

const std::vector<MouseButton>& InputCombination::mouseButtons() const
{
    return mMouseButtons;
}

std::vector<Key>& InputCombination::keys()
{
    return mKeys;
}

std::vector<GamepadButton>& InputCombination::gamepadButtons()
{
    return mGamepadButtons;
}

std::vector<MouseButton>& InputCombination::mouseButtons()
{
    return mMouseButtons;
}

bool InputCombination::pressed(const Window& window, const GamepadState* const gamepad) const
{
    for (const auto& key : mKeys)
    {
        if (!window->pressed(key))
        {
            return false;
        }
    }

    for (const auto& button : mMouseButtons)
    {
        if (!window->pressed(button))
        {
            return false;
        }
    }

    if (gamepad == nullptr)
    {
        // If there are gamepad buttons to be pressed, but no gamepad is connected, combination is not pressed.
        return mGamepadButtons.empty();
    }

    for (const auto& button : mGamepadButtons)
    {
        if (!gamepad->pressed(button))
        {
            return false;
        }
    }

    return true;
}
