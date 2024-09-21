#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/action.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputAction)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAction")
        .with(FieldsTrait{}.withField("combinations", &InputAction::combinations));
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

void InputAction::update(const core::io::Window& window)
{
    bool pressed = false;
    for (const auto& combination : combinations)
    {
        if (combination.pressed(window))
        {
            pressed = true;
            break;
        }
    }

    mJustPressed = pressed && !mPressed;
    mJustReleased = !pressed && mPressed;
    mPressed = pressed;
}
