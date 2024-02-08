#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;

using cubos::core::io::GamepadAxis;
using cubos::core::io::GamepadButton;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_EXTERNAL_IMPL(GamepadButton)
{
    return Type::create("GamepadButton")
        .with(EnumTrait{}
                  .withVariant<GamepadButton::Invalid>("Invalid")

                  .withVariant<GamepadButton::A>("A")
                  .withVariant<GamepadButton::B>("B")
                  .withVariant<GamepadButton::X>("X")
                  .withVariant<GamepadButton::Y>("Y")
                  .withVariant<GamepadButton::LBumper>("LBumper")
                  .withVariant<GamepadButton::RBumper>("RBumper")
                  .withVariant<GamepadButton::Back>("Back")
                  .withVariant<GamepadButton::Start>("Start")
                  .withVariant<GamepadButton::Guide>("Guide")
                  .withVariant<GamepadButton::LThumb>("LThumb")
                  .withVariant<GamepadButton::RThumb>("RThumb")
                  .withVariant<GamepadButton::Up>("Up")
                  .withVariant<GamepadButton::Right>("Right")
                  .withVariant<GamepadButton::Down>("Down")
                  .withVariant<GamepadButton::Left>("Left")

                  .withVariant<GamepadButton::Count>("Count"));
}

CUBOS_REFLECT_EXTERNAL_IMPL(GamepadAxis)
{
    return Type::create("GamepadAxis")
        .with(EnumTrait{}
                  .withVariant<GamepadAxis::Invalid>("Invalid")

                  .withVariant<GamepadAxis::LX>("LX")
                  .withVariant<GamepadAxis::LY>("LY")
                  .withVariant<GamepadAxis::RX>("RX")
                  .withVariant<GamepadAxis::RY>("RY")
                  .withVariant<GamepadAxis::LTrigger>("LTrigger")
                  .withVariant<GamepadAxis::RTrigger>("RTrigger")

                  .withVariant<GamepadAxis::Count>("Count"));
}
