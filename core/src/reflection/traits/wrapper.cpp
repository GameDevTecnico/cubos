#include <cubos/core/reflection/traits/wrapper.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(WrapperTrait)
{
    return Type::create("cubos::core::ecs::WrapperTrait");
}
