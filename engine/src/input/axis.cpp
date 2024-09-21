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
                  .withField("axes", &InputAxis::axes)
                  .withField("positive", &InputAxis::positive)
                  .withField("negative", &InputAxis::negative));
}

float InputAxis::value() const
{
    return mValue;
}

void InputAxis::update(const core::io::Window& window)
{
    float value = 0.0f;
    for (const auto& axis : axes)
    {
        value += axis.value(window);
    }

    positive.update(window);
    if (positive.pressed())
    {
        value += 1.0f;
    }

    negative.update(window);
    if (negative.pressed())
    {
        value -= 1.0f;
    }

    if (value > 1.0f)
    {
        value = 1.0f;
    }
    else if (value < -1.0f)
    {
        value = -1.0f;
    }

    mValue = value;
}
