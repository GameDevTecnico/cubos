#include <cubos/core/reflection/traits/constant.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(ConstantTrait)
{
    return Type::create("cubos::core::reflection::ConstantTrait");
}
