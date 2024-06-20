#include <cubos/core/tel/logging.hpp>

/// [Name declaration]
#include <cubos/core/reflection/reflect.hpp>

struct Name
{
    CUBOS_REFLECT;
    std::string inner;
};
/// [Name declaration]

/// [Name definition]
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Name)
{
    return Type::create("Name").with(
        StringConversionTrait{[](const void* instance) { return static_cast<const Name*>(instance)->inner; },
                              [](void* instance, const std::string& string) {
                                  static_cast<Name*>(instance)->inner = string;
                                  return !string.empty(); // Only accept non-empty strings.
                              }});
}
/// [Name definition]

/// [Accessing the trait]
int main()
{
    using cubos::core::reflection::reflect;

    const auto& nameType = reflect<Name>();
    CUBOS_ASSERT(nameType.has<StringConversionTrait>());
    const auto& stringConversion = nameType.get<StringConversionTrait>();
    /// [Accessing the trait]

    /// [Converting to a string]
    Name name{"Michael"};
    CUBOS_ASSERT(stringConversion.into(&name) == "Michael");
    /// [Converting to a string]

    /// [Converting from a string]
    CUBOS_ASSERT(stringConversion.from(&name, "John") == true); // Valid name, returns true
    CUBOS_ASSERT(name.inner == "John");
    CUBOS_ASSERT(stringConversion.from(&name, "") == false); // Invalid name, returns false
}
/// [Converting from a string]
