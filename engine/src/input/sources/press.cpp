#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/variant.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/sources/press.hpp>

using namespace cubos::engine;

using cubos::core::io::GamepadState;
using cubos::core::io::Window;

CUBOS_REFLECT_IMPL(cubos::engine::InputPressSource::Key)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputPressSource::Key")
        .with(FieldsTrait{}.withField("key", &InputPressSource::Key::key));
}

bool InputPressSource::Key::pressed(const core::io::Window& window) const
{
    return window->pressed(key);
}

CUBOS_REFLECT_IMPL(cubos::engine::InputPressSource::Mouse)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputPressSource::Mouse")
        .with(FieldsTrait{}.withField("button", &InputPressSource::Mouse::button));
}

bool InputPressSource::Mouse::pressed(const core::io::Window& window) const
{
    return window->pressed(button);
}

CUBOS_REFLECT_IMPL(cubos::engine::InputPressSource::Gamepad)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputPressSource::Gamepad")
        .with(FieldsTrait{}
                  .withField("index", &InputPressSource::Gamepad::index)
                  .withField("button", &InputPressSource::Gamepad::button));
}

bool InputPressSource::Gamepad::pressed(const core::io::Window& window) const
{
    GamepadState state;
    if (!window->gamepadState(index, state))
    {
        CUBOS_DEBUG("Gamepad {} is missing", index);
        return false;
    }

    return state.buttons[static_cast<int>(button)];
}

CUBOS_REFLECT_IMPL(cubos::engine::InputPressSource::AxisActivation)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputPressSource::AxisActivation")
        .with(FieldsTrait{}
                  .withField("axis", &InputPressSource::AxisActivation::axis)
                  .withField("activationStart", &InputPressSource::AxisActivation::activationStart)
                  .withField("activationEnd", &InputPressSource::AxisActivation::activationEnd));
}

bool InputPressSource::AxisActivation::pressed(const core::io::Window& window) const
{
    auto value = axis.value(window);

    return value >= activationStart && value <= activationEnd;
}

CUBOS_REFLECT_IMPL(cubos::engine::InputPressSource)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputPressSource")
        .with(FieldsTrait{}.withField("source", &InputPressSource::source));
}

bool InputPressSource::pressed(const Window& window) const
{
    return std::visit([&window](auto&& ax) { return ax.pressed(window); }, source);
}
