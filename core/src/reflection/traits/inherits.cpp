#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/inherits.hpp>

using namespace cubos::core::reflection;

const Type& InheritsTrait::base()
{
    return *mType;
}
