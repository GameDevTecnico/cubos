#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/bindings.hpp>

using cubos::core::io::Key;
using cubos::core::io::keyToString;
using cubos::core::io::Modifiers;
using cubos::core::io::modifiersToString;
using cubos::core::io::stringToKey;
using cubos::core::io::stringToModifiers;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputBindings)
{
    return core::reflection::Type::create("cubos::engine::InputBindings");
}

const std::unordered_map<std::string, InputAction>& InputBindings::actions() const
{
    return mActions;
}

const std::unordered_map<std::string, InputAxis>& InputBindings::axes() const
{
    return mAxes;
}

std::unordered_map<std::string, InputAction>& InputBindings::actions()
{
    return mActions;
}

std::unordered_map<std::string, InputAxis>& InputBindings::axes()
{
    return mAxes;
}
