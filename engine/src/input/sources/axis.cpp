#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/variant.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/sources/axis.hpp>

using namespace cubos::engine;

using cubos::core::io::GamepadState;
using cubos::core::io::Window;

CUBOS_REFLECT_IMPL(cubos::engine::InputAxisSource::Gamepad)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAxisSource::Gamepad")
        .with(FieldsTrait{}
                  .withField("index", &InputAxisSource::Gamepad::index)
                  .withField("axis", &InputAxisSource::Gamepad::axis)
                  .withField("deadzone", &InputAxisSource::Gamepad::deadzone));
}

float InputAxisSource::Gamepad::value(const core::io::Window& window) const
{
    GamepadState state;
    if (!window->gamepadState(index, state))
    {
        CUBOS_DEBUG("Gamepad {} is missing", index);
        return 0.0f;
    }

    float value = state.axes[static_cast<int>(axis)];

    if (value > -deadzone && value < deadzone)
    {
        return 0.0f;
    }

    return value;
}

CUBOS_REFLECT_IMPL(cubos::engine::InputAxisSource::Mouse)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAxisSource::Mouse")
        .with(FieldsTrait{}
                  .withField("axis", &InputAxisSource::Mouse::axis)
                  .withField("relative", &InputAxisSource::Mouse::relative));
}

float InputAxisSource::Mouse::value(const core::io::Window& window) const
{
    (void)window; // TODO: UNIMPLEMENTED
    return 0.0f;
}

CUBOS_REFLECT_IMPL(cubos::engine::InputAxisSource)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputAxisSource")
        .with(FieldsTrait{}.withField("axis", &InputAxisSource::source));
}

float InputAxisSource::value(const Window& window) const
{
    return std::visit([&window](auto&& ax) { return ax.value(window); }, source);
}
