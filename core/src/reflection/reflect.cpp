#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

auto cubos::core::reflection::makeUnnamedType(unsigned long id) -> const Type&
{
    return Type::unnamed(id);
}
