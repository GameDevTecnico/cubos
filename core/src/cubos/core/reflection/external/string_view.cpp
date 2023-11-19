#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(std::string_view)
{
    return Type::create("std::string_view")
        .with(StringConversionTrait{
            [](const void* instance) { return std::string{*static_cast<const std::string_view*>(instance)}; },
            [](void*, const std::string&) { return false; }});
}
