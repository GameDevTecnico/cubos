#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;

using cubos::core::data::old::Deserializer;
using cubos::core::data::old::Serializer;

using cubos::core::io::GamepadAxis;
using cubos::core::io::GamepadButton;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;

std::string io::gamepadButtonToString(GamepadButton button)
{
#define MAP_BUTTON_TO_STRING(button, string)                                                                           \
    case GamepadButton::button:                                                                                        \
        return string;
    switch (button)
    {
        MAP_BUTTON_TO_STRING(A, "A")
        MAP_BUTTON_TO_STRING(B, "B")
        MAP_BUTTON_TO_STRING(X, "X")
        MAP_BUTTON_TO_STRING(Y, "Y")
        MAP_BUTTON_TO_STRING(LBumper, "LBumper")
        MAP_BUTTON_TO_STRING(RBumper, "RBumper")
        MAP_BUTTON_TO_STRING(Back, "Back")
        MAP_BUTTON_TO_STRING(Start, "Start")
        MAP_BUTTON_TO_STRING(Guide, "Guide")
        MAP_BUTTON_TO_STRING(LThumb, "LThumb")
        MAP_BUTTON_TO_STRING(RThumb, "RThumb")
        MAP_BUTTON_TO_STRING(Up, "Up")
        MAP_BUTTON_TO_STRING(Right, "Right")
        MAP_BUTTON_TO_STRING(Down, "Down")
        MAP_BUTTON_TO_STRING(Left, "Left")
    default:
        return "Invalid";
    }
#undef MAP_KEY_TO_STRING
}

template <>
void data::old::serialize<GamepadButton>(Serializer& ser, const GamepadButton& obj, const char* name)
{
    ser.write(io::gamepadButtonToString(obj), name);
}

GamepadButton io::stringToGamepadButton(const std::string& str)
{
#define MAP_STRING_TO_BUTTON(string, button)                                                                           \
    if (str == (string))                                                                                               \
        return GamepadButton::button;
    MAP_STRING_TO_BUTTON("A", A)
    MAP_STRING_TO_BUTTON("B", B)
    MAP_STRING_TO_BUTTON("X", X)
    MAP_STRING_TO_BUTTON("Y", Y)
    MAP_STRING_TO_BUTTON("LBumper", LBumper)
    MAP_STRING_TO_BUTTON("RBumper", RBumper)
    MAP_STRING_TO_BUTTON("Back", Back)
    MAP_STRING_TO_BUTTON("Start", Start)
    MAP_STRING_TO_BUTTON("Guide", Guide)
    MAP_STRING_TO_BUTTON("LThumb", LThumb)
    MAP_STRING_TO_BUTTON("RThumb", RThumb)
    MAP_STRING_TO_BUTTON("Up", Up)
    MAP_STRING_TO_BUTTON("Right", Right)
    MAP_STRING_TO_BUTTON("Down", Down)
    MAP_STRING_TO_BUTTON("Left", Left)
    // else
    return GamepadButton::Invalid;
#undef MAP_STRING_TO_BUTTON
}

template <>
void data::old::deserialize<GamepadButton>(Deserializer& des, GamepadButton& obj)
{
    std::string axis;
    des.read(axis);
    obj = io::stringToGamepadButton(axis);
}

std::string io::gamepadAxisToString(GamepadAxis axis)
{
#define MAP_AXIS_TO_STRING(axis, string)                                                                               \
    case GamepadAxis::axis:                                                                                            \
        return string;
    switch (axis)
    {
        MAP_AXIS_TO_STRING(LX, "LX")
        MAP_AXIS_TO_STRING(LY, "LY")
        MAP_AXIS_TO_STRING(RX, "RX")
        MAP_AXIS_TO_STRING(RY, "RY")
        MAP_AXIS_TO_STRING(LTrigger, "LTrigger")
        MAP_AXIS_TO_STRING(RTrigger, "RTrigger")
    default:
        return "Invalid";
    }
#undef MAP_AXIS_TO_STRING
}

template <>
void data::old::serialize<GamepadAxis>(Serializer& ser, const GamepadAxis& obj, const char* name)
{
    ser.write(io::gamepadAxisToString(obj), name);
}

GamepadAxis io::stringToGamepadAxis(const std::string& str)
{
#define MAP_STRING_TO_AXIS(string, axis)                                                                               \
    if (str == (string))                                                                                               \
        return GamepadAxis::axis;
    MAP_STRING_TO_AXIS("LX", LX)
    MAP_STRING_TO_AXIS("LY", LY)
    MAP_STRING_TO_AXIS("RX", RX)
    MAP_STRING_TO_AXIS("RY", RY)
    MAP_STRING_TO_AXIS("LTrigger", LTrigger)
    MAP_STRING_TO_AXIS("RTrigger", RTrigger)
    // else
    return GamepadAxis::Invalid;
#undef MAP_STRING_TO_AXIS
}

template <>
void data::old::deserialize<GamepadAxis>(Deserializer& des, GamepadAxis& obj)
{
    std::string axis;
    des.read(axis);
    obj = io::stringToGamepadAxis(axis);
}

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
