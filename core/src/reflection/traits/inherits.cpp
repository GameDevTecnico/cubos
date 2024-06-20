#include <cubos/core/reflection/traits/inherits.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::reflection;

const Type& InheritsTrait::base()
{
    return *mType;
}
