#include <cmath>

#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/axis.hpp>

using cubos::core::io::GamepadAxis;
using cubos::core::io::Key;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputAxis)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAxis")
        .with(FieldsTrait{}
                  .withField("positive", &InputAxis::mPositive)
                  .withField("negative", &InputAxis::mNegative)
                  .withField("gamepadAxes", &InputAxis::mGamepadAxes));
}

const std::vector<InputCombination>& InputAxis::positive() const
{
    return mPositive;
}

const std::vector<InputCombination>& InputAxis::negative() const
{
    return mNegative;
}

const std::vector<GamepadAxis>& InputAxis::gamepadAxes() const
{
    return mGamepadAxes;
}

std::vector<InputCombination>& InputAxis::positive()
{
    return mPositive;
}

std::vector<InputCombination>& InputAxis::negative()
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
    mValue = value;

    if (std::abs(mValue) > 1.0F)
    {
        CUBOS_WARN("Axis value out of range: {}", mValue);
        mValue = mValue > 1.0F ? 1.0F : -1.0F;
    }
}
