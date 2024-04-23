#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::StringConversionTrait;

CUBOS_REFLECT_IMPL(StringConversionTrait)
{
    return Type::create("cubos::core::ecs::StringConversionTrait");
}

StringConversionTrait::StringConversionTrait(Into into, From from)
    : mInto{into}
    , mFrom{from}
{
}

std::string StringConversionTrait::into(const void* instance) const
{
    return mInto(instance);
}

bool StringConversionTrait::from(void* instance, const std::string& string) const
{
    return mFrom(instance, string);
}
