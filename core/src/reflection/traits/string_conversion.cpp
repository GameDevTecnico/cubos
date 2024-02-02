#include <cubos/core/reflection/traits/string_conversion.hpp>

using cubos::core::reflection::StringConversionTrait;

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
