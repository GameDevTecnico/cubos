#include <cubos/core/reflection/traits/inherits.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(InheritsTrait)
{
    return Type::create("cubos::core::ecs::InheritsTrait");
}

const Type& InheritsTrait::base() const
{
    return *mType;
}
