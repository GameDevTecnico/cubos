#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(std::string)
{
    return Type::create("std::string")
        .with(ConstructibleTrait::typed<std::string>().withBasicConstructors().build())
        .with(StringConversionTrait{[](const void* instance) { return *static_cast<const std::string*>(instance); },
                                    [](void* instance, const std::string& string) {
                                        *static_cast<std::string*>(instance) = string;
                                        return true;
                                    }});
}
