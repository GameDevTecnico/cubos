#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

auto cubos::core::reflection::makeUnnamedType(unsigned long long id, std::size_t size, std::size_t alignment,
                                              void (*destructor)(void*)) -> const Type&
{
    return Type::unnamed(id).with(ConstructibleTrait{size, alignment, destructor});
}
