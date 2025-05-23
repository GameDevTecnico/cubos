#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/data/transparent/string_hash.hpp>
#include <cubos/core/data/transparent/string_equal.hpp>

#include <cubos/engine/input/bindings.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputBindings)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputBindings")
        .with(FieldsTrait{}.withField("actions", &InputBindings::mActions).withField("axes", &InputBindings::mAxes));
}

const std::unordered_map<std::string, InputAction, cubos::core::data::StringTransparentHash, cubos::core::data::StringTransparentEqual>& InputBindings::actions() const
{
    return mActions;
}

const std::unordered_map<std::string, InputAxis, cubos::core::data::StringTransparentHash, cubos::core::data::StringTransparentEqual>& InputBindings::axes() const
{
    return mAxes;
}

std::unordered_map<std::string, InputAction, cubos::core::data::StringTransparentHash, cubos::core::data::StringTransparentEqual>& InputBindings::actions()
{
    return mActions;
}

std::unordered_map<std::string, InputAxis, cubos::core::data::StringTransparentHash, cubos::core::data::StringTransparentEqual>& InputBindings::axes()
{
    return mAxes;
}
