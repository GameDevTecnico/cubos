#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/primitive.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(std::string)
{
    return PrimitiveType::build("std::string").defaultConstructible<std::string>().get();
}
