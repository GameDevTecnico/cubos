#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/action.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputAction)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAction")
        .with(FieldsTrait{}.withField("combinations", &InputAction::mCombinations));
}

const std::vector<InputCombination>& InputAction::combinations() const
{
    return mCombinations;
}

std::vector<InputCombination>& InputAction::combinations()
{
    return mCombinations;
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
