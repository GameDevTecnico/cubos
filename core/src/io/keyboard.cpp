#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::io::Key;
using cubos::core::io::KeyWithModifiers;
using cubos::core::io::Modifiers;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_EXTERNAL_IMPL(Key)
{
    return Type::create("Key").with(EnumTrait{}
                                        .withVariant<Key::Invalid>("Invalid")

                                        .withVariant<Key::A>("A")
                                        .withVariant<Key::B>("B")
                                        .withVariant<Key::C>("C")
                                        .withVariant<Key::D>("D")
                                        .withVariant<Key::E>("E")
                                        .withVariant<Key::F>("F")
                                        .withVariant<Key::G>("G")
                                        .withVariant<Key::H>("H")
                                        .withVariant<Key::I>("I")
                                        .withVariant<Key::J>("J")
                                        .withVariant<Key::K>("K")
                                        .withVariant<Key::L>("L")
                                        .withVariant<Key::M>("M")
                                        .withVariant<Key::N>("N")
                                        .withVariant<Key::O>("O")
                                        .withVariant<Key::P>("P")
                                        .withVariant<Key::Q>("Q")
                                        .withVariant<Key::R>("R")
                                        .withVariant<Key::S>("S")
                                        .withVariant<Key::T>("T")
                                        .withVariant<Key::U>("U")
                                        .withVariant<Key::V>("V")
                                        .withVariant<Key::W>("W")
                                        .withVariant<Key::X>("X")
                                        .withVariant<Key::Y>("Y")
                                        .withVariant<Key::Z>("Z")
                                        .withVariant<Key::Num0>("Num0")
                                        .withVariant<Key::Num1>("Num1")
                                        .withVariant<Key::Num2>("Num2")
                                        .withVariant<Key::Num3>("Num3")
                                        .withVariant<Key::Num4>("Num4")
                                        .withVariant<Key::Num5>("Num5")
                                        .withVariant<Key::Num6>("Num6")
                                        .withVariant<Key::Num7>("Num7")
                                        .withVariant<Key::Num8>("Num8")
                                        .withVariant<Key::Num9>("Num9")
                                        .withVariant<Key::Escape>("Escape")
                                        .withVariant<Key::LControl>("LControl")
                                        .withVariant<Key::LShift>("LShift")
                                        .withVariant<Key::LAlt>("LAlt")
                                        .withVariant<Key::LSystem>("LSystem")
                                        .withVariant<Key::RControl>("RControl")
                                        .withVariant<Key::RShift>("RShift")
                                        .withVariant<Key::RAlt>("RAlt")
                                        .withVariant<Key::RSystem>("RSystem")
                                        .withVariant<Key::Menu>("Menu")
                                        .withVariant<Key::LBracket>("LBracket")
                                        .withVariant<Key::RBracket>("RBracket")
                                        .withVariant<Key::SemiColon>("SemiColon")
                                        .withVariant<Key::Comma>("Comma")
                                        .withVariant<Key::Period>("Period")
                                        .withVariant<Key::Quote>("Quote")
                                        .withVariant<Key::Slash>("Slash")
                                        .withVariant<Key::BackSlash>("BackSlash")
                                        .withVariant<Key::Tilde>("Tilde")
                                        .withVariant<Key::Equal>("Equal")
                                        .withVariant<Key::Dash>("Dash")
                                        .withVariant<Key::Space>("Space")
                                        .withVariant<Key::Return>("Return")
                                        .withVariant<Key::BackSpace>("BackSpace")
                                        .withVariant<Key::Tab>("Tab")
                                        .withVariant<Key::PageUp>("PageUp")
                                        .withVariant<Key::PageDown>("PageDown")
                                        .withVariant<Key::End>("End")
                                        .withVariant<Key::Home>("Home")
                                        .withVariant<Key::Insert>("Insert")
                                        .withVariant<Key::Delete>("Delete")
                                        .withVariant<Key::Add>("Add")
                                        .withVariant<Key::Subtract>("Subtract")
                                        .withVariant<Key::Multiply>("Multiply")
                                        .withVariant<Key::Divide>("Divide")
                                        .withVariant<Key::Left>("Left")
                                        .withVariant<Key::Right>("Right")
                                        .withVariant<Key::Up>("Up")
                                        .withVariant<Key::Down>("Down")
                                        .withVariant<Key::Numpad0>("Numpad0")
                                        .withVariant<Key::Numpad1>("Numpad1")
                                        .withVariant<Key::Numpad2>("Numpad2")
                                        .withVariant<Key::Numpad3>("Numpad3")
                                        .withVariant<Key::Numpad4>("Numpad4")
                                        .withVariant<Key::Numpad5>("Numpad5")
                                        .withVariant<Key::Numpad6>("Numpad6")
                                        .withVariant<Key::Numpad7>("Numpad7")
                                        .withVariant<Key::Numpad8>("Numpad8")
                                        .withVariant<Key::Numpad9>("Numpad9")
                                        .withVariant<Key::F1>("F1")
                                        .withVariant<Key::F2>("F2")
                                        .withVariant<Key::F3>("F3")
                                        .withVariant<Key::F4>("F4")
                                        .withVariant<Key::F5>("F5")
                                        .withVariant<Key::F6>("F6")
                                        .withVariant<Key::F7>("F7")
                                        .withVariant<Key::F8>("F8")
                                        .withVariant<Key::F9>("F9")
                                        .withVariant<Key::F10>("F10")
                                        .withVariant<Key::F11>("F11")
                                        .withVariant<Key::F12>("F12")
                                        .withVariant<Key::Pause>("Pause")

                                        .withVariant<Key::Count>("Count"));
}

CUBOS_REFLECT_EXTERNAL_IMPL(Modifiers)
{
    return Type::create("Modifiers")
        .with(EnumTrait{}
                  .withVariant<Modifiers::None>("None")
                  .withVariant<Modifiers::Control>("Control")
                  .withVariant<Modifiers::Shift>("Shift")
                  .withVariant<Modifiers::Alt>("Alt")
                  .withVariant<Modifiers::System>("System"));
}

CUBOS_REFLECT_IMPL(KeyWithModifiers)
{
    return Type::create("cubos::core::io::KeyWithModifiers")
        .with(FieldsTrait{}
                  .withField("key", &KeyWithModifiers::key)
                  .withField("modifiers", &KeyWithModifiers::modifiers));
}
