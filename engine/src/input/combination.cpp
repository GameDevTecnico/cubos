#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/combination.hpp>

using cubos::core::io::GamepadButton;
using cubos::core::io::GamepadState;
using cubos::core::io::Key;
using cubos::core::io::MouseButton;
using cubos::core::io::Window;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputCombination)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputCombination")
        .with(FieldsTrait{}.withField("sources", &InputCombination::mSources));
}

bool InputCombination::pressed(const Window& window) const
{
    for (const auto& source : mSources)
    {
        if (!source.pressed(window))
        {
            return false;
        }
    }

    return true;
}
