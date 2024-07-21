#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/inherits.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(InheritsTrait)
{
    return Type::create("cubos::core::ecs::InheritsTrait");
}

const Type& InheritsTrait::base()
{
    return *mType;
}
