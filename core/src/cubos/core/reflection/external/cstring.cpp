#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(const char*)
{
    return Type::create("const char*")
        .with(StringConversionTrait{
            [](const void* instance) { return std::string{*static_cast<const char* const*>(instance)}; },
            [](void*, const std::string&) { return false; }});
}

CUBOS_REFLECT_EXTERNAL_IMPL(char*)
{
    return Type::create("char*").with(
        StringConversionTrait{[](const void* instance) { return std::string{*static_cast<char* const*>(instance)}; },
                              [](void*, const std::string&) { return false; }});
}
